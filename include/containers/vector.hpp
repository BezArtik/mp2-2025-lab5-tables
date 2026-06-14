#pragma once

#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <memory>
#include <iterator>
#include <cstddef>
#include <concepts>

namespace containers {

template<typename T>
class Vector {
public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    template <typename U>
    class VectorIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = U;
        using difference_type   = ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;

        VectorIterator() noexcept = default;
        VectorIterator(pointer ptr = nullptr) noexcept : ptr_(ptr) {}
        template <typename V>
            requires std::same_as<std::remove_const_t<U>, std::remove_const_t<V>>
        VectorIterator(const VectorIterator<V>& other) noexcept : ptr_(other.ptr_) {}

        reference operator*() const noexcept { return *ptr_; }
        pointer operator->() const noexcept { return ptr_; }
        reference operator[](difference_type n) const noexcept { return ptr_[n]; }
        VectorIterator& operator++() noexcept { ++ptr_; return *this; }
        VectorIterator operator++(int) noexcept { VectorIterator temp = *this; ++ptr_; return temp; }
        VectorIterator& operator--() noexcept { --ptr_; return *this; }
        VectorIterator operator--(int) noexcept { VectorIterator temp = *this; --ptr_; return temp; }
        VectorIterator operator+(difference_type n) const noexcept { return VectorIterator(ptr_ + n); }
        VectorIterator operator-(difference_type n) const noexcept { return VectorIterator(ptr_ - n); }
        VectorIterator& operator+=(difference_type n) noexcept { ptr_ += n; return *this; }
        VectorIterator& operator-=(difference_type n) noexcept { ptr_ -= n; return *this; }
        difference_type operator-(const VectorIterator& other) const noexcept { return ptr_ - other.ptr_; }

        bool operator==(const VectorIterator& other) const noexcept { return ptr_ == other.ptr_; }
        bool operator!=(const VectorIterator& other) const noexcept { return ptr_ != other.ptr_; }
        bool operator<(const VectorIterator& other) const noexcept { return ptr_ < other.ptr_; }
        bool operator<=(const VectorIterator& other) const noexcept { return ptr_ <= other.ptr_; }
        bool operator>(const VectorIterator& other) const noexcept { return ptr_ > other.ptr_; }
        bool operator>=(const VectorIterator& other) const noexcept { return ptr_ >= other.ptr_; }

    private:
        pointer ptr_{ nullptr };
        friend class Vector<T>;
        template<typename V>
        friend class VectorIterator;
    };

    using iterator        = VectorIterator<value_type>;
    using const_iterator  = VectorIterator<const value_type>;

    Vector() noexcept = default;

    Vector(size_type n) {
        allocate_and_init(n, [](pointer p, size_type n) {
            std::uninitialized_value_construct_n(p, n);
            });
    }

    Vector(size_type n, const_reference value) {
        allocate_and_init(n, [&](pointer p, size_type n) {
            std::uninitialized_fill_n(p, n, value);
            });
    }

    Vector(const Vector& other) {
        if (!other.empty()) {
            allocate_and_copy(other.begin(), other.end());
        }
    }

    Vector(Vector&& other) noexcept
        : buf_(std::move(other.buf_)) {
    }

    template<std::input_iterator Iter>
    Vector(Iter first, Iter last) {
        const auto n = std::distance(first, last);
        if (n > 0) {
            allocate_and_copy(first, last);
        }
    }

    Vector(std::initializer_list<value_type> init)
        : Vector(init.begin(), init.end()) {
    }

    ~Vector() {
        clear();
        buf_.deallocate_buffer();
    }

    Vector& operator=(const Vector& other) {
        Vector tmp(other);
        swap(tmp);
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        Vector tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    reference operator[](size_type index) noexcept { return buf_[index]; }
    const_reference operator[](size_type index) const noexcept { return buf_[index]; }

    reference at(size_type index) {
        if (index >= size()) {
            throw std::out_of_range("Vector::at - index out of range");
        }
        return buf_[index];
    }

    const_reference at(size_type index) const {
        if (index >= size()) {
            throw std::out_of_range("Vector::at - index out of range");
        }
        return buf_[index];
    }

    reference front() noexcept { return *buf_.begin_; }
    const_reference front() const noexcept { return *buf_.begin_; }
    reference back() noexcept { return *(buf_.end_ - 1); }
    const_reference back() const noexcept { return *(buf_.end_ - 1); }

    pointer data() noexcept { return buf_.data(); }
    const_pointer data() const noexcept { return buf_.data(); }

    iterator begin() noexcept { return iterator(buf_.begin_); }
    iterator end() noexcept { return iterator(buf_.end_); }
    const_iterator begin() const noexcept { return const_iterator(buf_.begin_); }
    const_iterator end() const noexcept { return const_iterator(buf_.end_); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    size_type size() const noexcept { return buf_.size(); }
    size_type capacity() const noexcept { return buf_.capacity(); }
    bool empty() const noexcept { return buf_.empty(); }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity()) return;
        reallocate(new_cap);
    }

    void resize(size_type count, const_reference value) {
        if (count < size()) {
            std::destroy(buf_.begin_ + count, buf_.end_);
            buf_.end_ = buf_.begin_ + count;
        } else if (count > size()) {
            if (count > capacity()) {
                reserve(count);
            }
            std::uninitialized_fill_n(buf_.end_, count - size(), value);
            buf_.end_ = buf_.begin_ + count;
        }
    }

    void resize(size_type count) {
        resize(count, value_type());
    }

    void shrink_to_fit() {
        if (capacity() == size() || empty()) return;
        reallocate(size());
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (buf_.end_ == buf_.capacity_) {
            reallocate_and_construct_at(size(), std::forward<Args>(args)...);
        } else {
            std::construct_at(buf_.end_, std::forward<Args>(args)...);
            ++buf_.end_;
        }
    }

    void push_back(const_reference value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        const auto index = pos - cbegin();
        if (index == size()) {
            emplace_back(std::forward<Args>(args)...);
            return iterator(buf_.begin_ + index);
        }

        if (buf_.end_ == buf_.capacity_) {
            reallocate_and_construct_at(index, std::forward<Args>(args)...);
        } else {
            std::construct_at(buf_.end_, std::move(*(buf_.end_ - 1)));
            ++buf_.end_;
            std::move_backward(buf_.begin_ + index, buf_.end_ - 2, buf_.end_ - 1);
            std::destroy_at(buf_.begin_ + index);
            std::construct_at(buf_.begin_ + index, std::forward<Args>(args)...);
        }
        return iterator(buf_.begin_ + index);
    }

    iterator insert(const_iterator pos, const_reference value) {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value) {
        return emplace(pos, std::move(value));
    }

    iterator erase(const_iterator pos) {
        const auto index = pos - cbegin();
        auto erase_pos = buf_.begin_ + index;
        if (erase_pos != buf_.end_ - 1) {
            std::move(erase_pos + 1, buf_.end_, erase_pos);
        }
        --buf_.end_;
        std::destroy_at(buf_.end_);
        return iterator(erase_pos);
    }

    void pop_back() noexcept {
        --buf_.end_;
        std::destroy_at(buf_.end_);
    }

    void clear() noexcept {
        std::destroy(buf_.begin_, buf_.end_);
        buf_.end_ = buf_.begin_;
    }

    void swap(Vector& rhs) noexcept {
        buf_.swap(rhs.buf_);
    }

    bool operator==(const Vector& rhs) const noexcept {
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    }

    auto operator<=>(const Vector& rhs) const noexcept {
        return std::lexicographical_compare_three_way(
            begin(), end(), rhs.begin(), rhs.end());
    }

private:
    template<typename Func>
    void allocate_and_init(size_type n, Func init_func) {
        if (n == 0) return;
        ScopedBuffer scoped(n);
        init_func(scoped.buf_.begin_, n);
        scoped.buf_.end_ = scoped.buf_.begin_ + n;
        buf_ = std::move(scoped.buf_);
    }

    template<typename Iter>
    void allocate_and_copy(Iter first, Iter last) {
        const auto n = std::distance(first, last);
        if (n <= 0) return;
        ScopedBuffer scoped(static_cast<size_type>(n));
        scoped.buf_.end_ = std::uninitialized_copy(first, last, scoped.buf_.begin_);
        buf_ = std::move(scoped.buf_);
    }

    void reallocate(size_type new_cap) {
        ScopedBuffer scoped(new_cap);
        scoped.buf_.end_ = std::uninitialized_move(buf_.begin_, buf_.end_, scoped.buf_.begin_);
        std::destroy(buf_.begin_, buf_.end_);
        buf_.deallocate_buffer();
        buf_ = std::move(scoped.buf_);
    }

    template<typename... Args>
    void reallocate_and_construct_at(size_type index, Args&&... args) {
        const auto new_cap = capacity() * 2 + 1;
        ScopedBuffer scoped(new_cap);

        std::construct_at(scoped.buf_.begin_ + index, std::forward<Args>(args)...);
        std::uninitialized_move(buf_.begin_, buf_.begin_ + index, scoped.buf_.begin_);
        std::uninitialized_move(buf_.begin_ + index, buf_.end_, scoped.buf_.begin_ + index + 1);

        scoped.buf_.end_ = scoped.buf_.begin_ + size() + 1;

        buf_ = std::move(scoped.buf_);
    }

    struct Buffer {
        pointer begin_{ nullptr };
        pointer end_{ nullptr };
        pointer capacity_{ nullptr };

        Buffer() = default;
        ~Buffer() = default;

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept
            : begin_(std::exchange(other.begin_, nullptr))
            , end_(std::exchange(other.end_, nullptr))
            , capacity_(std::exchange(other.capacity_, nullptr)) {
        }

        Buffer& operator=(Buffer&& other) noexcept {
            if (this == &other) return *this;
            begin_ = std::exchange(other.begin_, nullptr);
            end_ = std::exchange(other.end_, nullptr);
            capacity_ = std::exchange(other.capacity_, nullptr);
            return *this;
        }

        void allocate_buffer(size_type n) {
            if (n > 0) {
                begin_ = allocate(n);
                capacity_ = begin_ + n;
                end_ = begin_;
            }
        }

        void deallocate_buffer() noexcept {
            if (begin_) {
                deallocate(begin_, capacity());
                begin_ = end_ = capacity_ = nullptr;
            }
        }

        size_type size() const noexcept { return end_ - begin_; }
        size_type capacity() const noexcept { return capacity_ - begin_; }
        pointer data() noexcept { return begin_; }
        const_pointer data() const noexcept { return begin_; }
        bool empty() const noexcept { return begin_ == end_; }
        reference operator[](size_type index) noexcept { return begin_[index]; }
        const_reference operator[](size_type index) const noexcept { return begin_[index]; }

        void swap(Buffer& rhs) noexcept {
            using std::swap;
            swap(begin_, rhs.begin_);
            swap(end_, rhs.end_);
            swap(capacity_, rhs.capacity_);
        }

    private:
        static pointer allocate(size_type n) {
            return n != 0 ? static_cast<pointer>(::operator new(n * sizeof(value_type))) : nullptr;
        }

        static void deallocate(pointer ptr, size_type n) noexcept {
            if (ptr && n > 0) {
                ::operator delete(ptr, n * sizeof(value_type));
            }
        }
    } buf_{};

    struct ScopedBuffer {
        Buffer buf_;

        ScopedBuffer(size_type n) {
            buf_.allocate_buffer(n);
        }

        ~ScopedBuffer() {
            buf_.deallocate_buffer();
        }

        ScopedBuffer(const ScopedBuffer&) = delete;
        ScopedBuffer& operator=(const ScopedBuffer&) = delete;
    };
};

template <typename T>
void swap(Vector<T>& lhs, Vector<T>& rhs) noexcept {
    lhs.swap(rhs);
}

}