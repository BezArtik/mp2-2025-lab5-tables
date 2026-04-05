#pragma once

#include <initializer_list>
#include <exception>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <memory>
#include <iterator>
#include <compare>

namespace containers {

template<typename T>
class Vector {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() = default;
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;
        Iterator(Iterator&&) = default;
        Iterator& operator=(Iterator&&) = default;
        ~Iterator() = default;
        Iterator(pointer ptr = nullptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }
        reference operator[](difference_type n) const noexcept { return ptr_[n]; }
        Iterator& operator++() noexcept { ++ptr_; return *this; }
        Iterator operator++(int) noexcept { Iterator temp = *this; ++ptr_; return temp; }
        Iterator& operator--() noexcept { --ptr_; return *this; }
        Iterator operator--(int) noexcept { Iterator temp = *this; --ptr_; return temp; }
        Iterator operator+(difference_type n) const noexcept { return Iterator(ptr_ + n); }
        Iterator operator-(difference_type n) const noexcept { return Iterator(ptr_ - n); }
        Iterator& operator+=(difference_type n) noexcept { ptr_ += n; return *this; }
        Iterator& operator-=(difference_type n) noexcept { ptr_ -= n; return *this; }
        difference_type operator-(const Iterator& other) const noexcept { return ptr_ - other.ptr_; }

        auto operator<=>(const Iterator&) const noexcept = default;

    private:
        pointer ptr_;
        friend class Vector;
    };

    class ConstIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ConstIterator() = default;
        ConstIterator(const ConstIterator&) = default;
        ConstIterator& operator=(const ConstIterator&) = default;
        ConstIterator(ConstIterator&&) = default;
        ConstIterator& operator=(ConstIterator&&) = default;
        ~ConstIterator() = default;
        ConstIterator(const Iterator& it) : ptr_(&(*it)) {}
        ConstIterator(const_pointer ptr = nullptr) : ptr_(ptr) {}

        const_reference operator*() const { return *ptr_; }
        const_pointer operator->() const { return ptr_; }
        const_reference operator[](difference_type n) const noexcept { return ptr_[n]; }
        ConstIterator& operator++() noexcept { ++ptr_; return *this; }
        ConstIterator operator++(int) noexcept { ConstIterator temp = *this; ++ptr_; return temp; }
        ConstIterator& operator--() noexcept { --ptr_; return *this; }
        ConstIterator operator--(int) noexcept { ConstIterator temp = *this; --ptr_; return temp; }
        ConstIterator operator+(difference_type n) const noexcept { return ConstIterator(ptr_ + n); }
        ConstIterator operator-(difference_type n) const noexcept { return ConstIterator(ptr_ - n); }
        ConstIterator& operator+=(difference_type n) noexcept { ptr_ += n; return *this; }
        ConstIterator& operator-=(difference_type n) noexcept { ptr_ -= n; return *this; }
        difference_type operator-(const ConstIterator& other) const noexcept { return ptr_ - other.ptr_; }

        auto operator<=>(const ConstIterator&) const noexcept = default;

    private:
        const_pointer ptr_;
        friend class Vector;
    };

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    Vector() noexcept {
        buf_.begin_ = buf_.end_ = buf_.capacity_ = nullptr;
    }

    Vector(size_type n) {
        allocate_and_construct_n(n, value_type());
    }

    Vector(size_type n, const_reference value) {
        allocate_and_construct_n(n, value);
    }

    Vector(const Vector& other) {
        allocate_and_copy(other.begin(), other.end());
    }

    Vector(Vector&& other) noexcept : buf_(std::move(other.buf_)) {
        other.buf_.begin_ = other.buf_.end_ = other.buf_.capacity_ = nullptr;
    }

    template<typename Iter>
    Vector(Iter first, Iter last) {
        allocate_and_copy(first, last);
    }

    Vector(std::initializer_list<value_type> init)
        : Vector(init.begin(), init.end()) {
    }

    ~Vector() {
        clear();
        deallocate(buf_.begin_, capacity());
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }
        if (capacity() >= other.size()) {
            pointer new_end = std::uninitialized_copy(other.begin(), other.end(), buf_.begin_);
            std::destroy(buf_.end_, new_end);
            buf_.end_ = new_end;
        } else {
            Vector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        if (capacity() >= other.size()) {
            pointer new_end = std::uninitialized_move(other.begin(), other.end(), buf_.begin_);
            std::destroy(buf_.end_, new_end);
            buf_.end_ = new_end;
            other.clear();
        } else {
            Vector tmp(std::move(other));
            swap(tmp);
        }
        return *this;
    }

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
            construct_range(buf_.end_, buf_.begin_ + count, value);
            buf_.end_ = buf_.begin_ + count;
        }
    }

    void resize(size_type count) {
        resize(count, value_type());
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (buf_.end_ == buf_.capacity_) {
            reallocate_and_construct_at(size(), std::forward<Args>(args)...);
        } else {
            construct_at(buf_.end_, std::forward<Args>(args)...);
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
        if (pos < cbegin() || pos > cend()) {
            throw std::out_of_range("Vector::emplace - iterator out of range");
        }
        size_type index = pos - cbegin();

        if (buf_.end_ == buf_.capacity_) {
            reallocate_and_construct_at(index, std::forward<Args>(args)...);
        } else {
            if (index < size()) {
                construct_at(buf_.end_, std::move_if_noexcept(*(buf_.end_ - 1)));
                ++buf_.end_;

                std::move_backward(buf_.begin_ + index, buf_.end_ - 2, buf_.end_ - 1);

                destroy_at(buf_.begin_ + index);
                construct_at(buf_.begin_ + index, std::forward<Args>(args)...);
            } else {
                construct_at(buf_.end_, std::forward<Args>(args)...);
                ++buf_.end_;
            }
        }

        return iterator(buf_.begin_ + index);
    }

    iterator insert(const_iterator pos, const_reference value) {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value) {
        return emplace(pos, std::move(value));
    }

    void shrink_to_fit() {
        if (capacity() == size() || empty()) {
            return;
        }
        reallocate(size());
    }

    iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<value_type>) {
        pointer erase_pos = buf_.begin_ + (pos - cbegin());
        if (erase_pos != buf_.end_ - 1) {
            std::move(erase_pos + 1, buf_.end_, erase_pos);
        }
        --buf_.end_;
        destroy_at(buf_.end_);
        return iterator(erase_pos);
    }

    void pop_back() noexcept {
        --buf_.end_;
        destroy_at(buf_.end_);
    }

    void clear() noexcept {
        std::destroy(buf_.begin_, buf_.end_);
        buf_.end_ = buf_.begin_;
    }

    size_type size() const noexcept { return buf_.size(); }
    size_type capacity() const noexcept { return buf_.capacity(); }
    bool empty() const noexcept { return buf_.empty(); }

    reference operator[](size_type index) noexcept { return buf_.begin_[index]; }
    const_reference operator[](size_type index) const noexcept { return buf_.begin_[index]; }

    reference at(size_type index) {
        if (index >= size()) throw std::out_of_range("Vector::at - index out of range");
        return buf_.begin_[index];
    }

    const_reference at(size_type index) const {
        if (index >= size()) throw std::out_of_range("Vector::at - index out of range");
        return buf_.begin_[index];
    }

    pointer data() noexcept { return buf_.data(); }
    const_pointer data() const noexcept { return buf_.data(); }

    reference front() noexcept { return buf_.begin_[0]; }
    const_reference front() const noexcept { return buf_.begin_[0]; }
    reference back() noexcept { return *(buf_.end_ - 1); }
    const_reference back() const noexcept { return *(buf_.end_ - 1); }

    const_iterator begin() const noexcept { return const_iterator(buf_.begin_); }
    const_iterator end() const noexcept { return const_iterator(buf_.end_); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    iterator begin() noexcept { return iterator(buf_.begin_); }
    iterator end() noexcept { return iterator(buf_.end_); }

    void swap(Vector& rhs) noexcept {
        buf_.swap(buf_, rhs.buf_);
    }

private:

    struct Buffer {
        pointer begin_ = nullptr;
        pointer end_ = nullptr;
        pointer capacity_ = nullptr;

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
            if (this == &other) {
                return *this;
            }
            begin_ = std::exchange(other.begin_, nullptr);
            end_ = std::exchange(other.end_, nullptr);
            capacity_ = std::exchange(other.capacity_, nullptr);
            return *this;
        }

        size_type size() const noexcept { return end_ - begin_; }
        size_type capacity() const noexcept { return capacity_ - begin_; }
        pointer data() noexcept { return begin_; }
        const_pointer data() const noexcept { return begin_; }
        bool empty() const noexcept { return begin_ == end_; }

        void swap(Buffer& lhs, Buffer& rhs) noexcept {
            using std::swap;
            swap(lhs.begin_, rhs.begin_);
            swap(lhs.end_, rhs.end_);
            swap(lhs.capacity_, rhs.capacity_);
        }
    } buf_;

    pointer allocate(size_type n) {
        return n != 0 ? static_cast<pointer>(::operator new(n * sizeof(value_type))) : nullptr;
    }

    void deallocate(pointer ptr, size_type n) noexcept {
        if (ptr && n > 0) {
            ::operator delete(ptr, n * sizeof(value_type));
        }
    }

    template<typename... Args>
    void construct_at(pointer ptr, Args&&... args) {
        ::new (static_cast<void*>(ptr)) value_type(std::forward<Args>(args)...);
    }

    void destroy_at(pointer ptr) noexcept {
        ptr->~value_type();
    }

    template<typename Iter>
    void construct_range(pointer first, pointer last, Iter iter) {
        auto current = first;
        try {
            current = std::uninitialized_copy_n(iter, last - first, first);
        } catch (...) {
            std::destroy(first, current);
            throw;
        }
    }

    void construct_range(pointer first, pointer last, const_reference value) {
        auto current = first;
        try {
            current = std::uninitialized_fill_n(first, last - first, value);
        } catch (...) {
            std::destroy(first, current);
            throw;
        }
    }

    void allocate_and_construct_n(size_type n, const_reference value) {
        if (n == 0) {
            buf_.begin_ = buf_.end_ = buf_.capacity_ = nullptr;
            return;
        }

        buf_.begin_ = allocate(n);
        buf_.capacity_ = buf_.begin_ + n;
        buf_.end_ = buf_.begin_;

        try {
            construct_range(buf_.begin_, buf_.begin_ + n, value);
            buf_.end_ = buf_.begin_ + n;
        } catch (...) {
            deallocate(buf_.begin_, n);
            throw;
        }
    }

    template<typename Iter>
    void allocate_and_copy(Iter first, Iter last) {
        auto n = std::distance(first, last);
        if (n == 0) {
            buf_.begin_ = buf_.end_ = buf_.capacity_ = nullptr;
            return;
        }

        buf_.begin_ = allocate(static_cast<size_type>(n));
        buf_.capacity_ = buf_.begin_ + n;
        buf_.end_ = buf_.begin_;

        try {
            construct_range(buf_.begin_, buf_.begin_ + n, first);
            buf_.end_ = buf_.begin_ + n;
        } catch (...) {
            deallocate(buf_.begin_, static_cast<size_type>(n));
            throw;
        }
    }

    void reallocate(size_type new_cap) {
        pointer new_begin = allocate(new_cap);
        pointer new_end = new_begin;

        try {
            new_end = std::uninitialized_move(buf_.begin_, buf_.end_, new_begin);
        } catch (...) {
            deallocate(new_begin, new_cap);
            throw;
        }

        std::destroy(buf_.begin_, buf_.end_);
        deallocate(buf_.begin_, capacity());

        buf_.begin_ = new_begin;
        buf_.end_ = new_end;
        buf_.capacity_ = new_begin + new_cap;
    }

    template<typename... Args>
    void reallocate_and_construct_at(size_type index, Args&&... args) {
        size_type new_cap = capacity() == 0 ? 1 : capacity() * 2;
        pointer new_begin = allocate(new_cap);
        pointer new_end = new_begin;

        try {
            new_end = std::uninitialized_move(buf_.begin_, buf_.begin_ + index, new_begin);
            construct_at(new_end, std::forward<Args>(args)...);
            ++new_end;
            new_end = std::uninitialized_move(buf_.begin_ + index, buf_.end_, new_end);
        } catch (...) {
            std::destroy(new_begin, new_end);
            deallocate(new_begin, new_cap);
            throw;
        }

        std::destroy(buf_.begin_, buf_.end_);
        deallocate(buf_.begin_, capacity());

        buf_.begin_ = new_begin;
        buf_.end_ = new_end;
        buf_.capacity_ = new_begin + new_cap;
    }
};

template <typename T>
void swap(Vector<T>& lhs, Vector<T>& rhs) noexcept {
    lhs.swap(rhs);
}

}