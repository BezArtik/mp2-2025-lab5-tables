#pragma once
#include <utility>
#include <stdexcept>
#include <initializer_list>
#include <type_traits>
#include <memory>
#include <iterator>
#include <compare>
#include <algorithm>

namespace containers {

template <typename T>
class List {
    struct Node;
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
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator() = default;
        Iterator(Node* node = nullptr) : curr_(node) {}

        reference operator*() const noexcept { return curr_->data_; }
        pointer operator->() const noexcept { return &curr_->data_; }
        Iterator& operator++() noexcept {
            curr_ = curr_->next_;
            return *this;
        }
        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        Iterator& operator--() noexcept {
            curr_ = curr_->prev_;
            return *this;
        }
        Iterator operator--(int) noexcept {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }
        auto operator<=>(const Iterator& other) const noexcept = default;

    private:
        Node* curr_;
        friend class List;
        friend class ConstIterator;
    };

    class ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;


        ConstIterator() = default;
        ConstIterator(const Node* node = nullptr) : curr_(node) {}
        ConstIterator(const Iterator& it) noexcept : curr_(it.curr_) {}

        const_reference operator*() const noexcept { return curr_->data_; }
        const_pointer operator->() const noexcept { return &curr_->data_; }
        ConstIterator& operator++() noexcept {
            curr_ = curr_->next_;
            return *this;
        }
        ConstIterator operator++(int) noexcept {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        ConstIterator& operator--() noexcept {
            curr_ = curr_->prev_;
            return *this;
        }
        ConstIterator operator--(int) noexcept {
            ConstIterator tmp = *this;
            --(*this);
            return tmp;
        }
        auto operator<=>(const ConstIterator& other) const noexcept = default;

    private:
        const Node* curr_;
        friend class List;
    };

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    List() : size_(0) {
        sentinel_.next_ = sentinel_.prev_ = &sentinel_;
    }

    template <std::input_iterator Iter>
    List(Iter first, Iter last) : List() {
        insert(first, last);
    }

    List(std::initializer_list<value_type> init)
        : List(init.begin(), init.end()) {
    }

    List(const List& other)
        : size_(0) {
        sentinel_.next_ = sentinel_.prev_ = &sentinel_;
        Node* curr = other.sentinel_.next_;
        Node* copy_prev = &sentinel_;
        try {
            while (curr != &other.sentinel_) {
                Node* p = create_node(curr->data_);
                copy_prev->next_ = p;
                p->prev_ = copy_prev;
                p->next_ = &sentinel_;
                copy_prev = p;

                curr = curr->next_;
                ++size_;
            }
        }
        catch (...) {
            clear();
            throw;
        }

        sentinel_.prev_ = copy_prev;
        copy_prev->next_ = &sentinel_;
    }

    List(List&& other) noexcept
        : size_(other.size_) {
        if (!other.empty()) {
            sentinel_.next_ = other.sentinel_.next_;
            sentinel_.prev_ = other.sentinel_.prev_;
            sentinel_.next_->prev_ = &sentinel_;
            sentinel_.prev_->next_ = &sentinel_;

            other.sentinel_.next_ = &other.sentinel_;
            other.sentinel_.prev_ = &other.sentinel_;
            other.size_ = 0;
        } else {
            sentinel_.next_ = sentinel_.prev_ = &sentinel_;
        }
    }

    List& operator=(const List& other) {
        List tmp(other);
        swap(tmp);
        return *this;
    }

    List& operator=(List&& other) noexcept {
        List tmp(std::move(other));
        swap(tmp);
        return *this;
    }
    ~List() { clear(); }

    void clear() noexcept {
        Node* curr = sentinel_.next_;
        while (curr != &sentinel_) {
            Node* next = curr->next_;
            destroy_node(curr);
            curr = next;
        }
        sentinel_.next_ = sentinel_.prev_ = &sentinel_;
        size_ = 0;
    }


    bool            empty() const noexcept { return size_ == 0; }
    size_type       size()  const noexcept { return size_; }
    reference       front()       noexcept { return sentinel_.next_->data_; }
    const_reference front() const noexcept { return sentinel_.next_->data_; }
    reference       back()        noexcept { return sentinel_.prev_->data_; }
    const_reference back()  const noexcept { return sentinel_.prev_->data_; }

    iterator        begin()         noexcept { return iterator(sentinel_.next_); }
    iterator        end()           noexcept { return iterator(&sentinel_); }
    const_iterator  begin()   const noexcept { return const_iterator(sentinel_.next_); }
    const_iterator  end()     const noexcept { return const_iterator(&sentinel_); }
    const_iterator  cbegin()  const noexcept { return const_iterator(sentinel_.next_); }
    const_iterator  cend()    const noexcept { return const_iterator(&sentinel_); }

    void swap(List& rhs) noexcept {
        using std::swap;
        swap(sentinel_.next_, rhs.sentinel_.next_);
        swap(sentinel_.prev_, rhs.sentinel_.prev_);
        swap(size_, rhs.size_);

        if (!empty()) {
            sentinel_.next_->prev_ = &sentinel_;
            sentinel_.prev_->next_ = &sentinel_;
        } else {
            sentinel_.next_ = &sentinel_;
            sentinel_.prev_ = &sentinel_;
        }

        if (!rhs.empty()) {
            rhs.sentinel_.next_->prev_ = &rhs.sentinel_;
            rhs.sentinel_.prev_->next_ = &rhs.sentinel_;
        } else {
            rhs.sentinel_.next_ = &rhs.sentinel_;
            rhs.sentinel_.prev_ = &rhs.sentinel_;
        }
    }

    template <std::input_iterator Iter>
    void insert(Iter first, Iter last) {
        std::copy(first, last, std::back_inserter(*this));
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        Node* p = create_node(std::forward<Args>(args)...);
        p->next_ = &sentinel_;
        p->prev_ = sentinel_.prev_;
        sentinel_.prev_->next_ = p;
        sentinel_.prev_ = p;
        ++size_;
    }

    template<typename... Args>
    void emplace_front(Args&&... args) {
        Node* p = create_node(std::forward<Args>(args)...);
        p->next_ = sentinel_.next_;
        p->prev_ = &sentinel_;
        sentinel_.next_->prev_ = p;
        sentinel_.next_ = p;
        ++size_;
    }

    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args) {
        Node* p = create_node(std::forward<Args>(args)...);
        Node* next_node = pos.curr_;
        Node* prev_node = next_node->prev_;

        p->next_ = next_node;
        p->prev_ = prev_node;
        prev_node->next_ = p;
        next_node->prev_ = p;

        ++size_;
        return iterator(p);
    }

    void push_back(const_reference data) { emplace_back(data); }
    void push_front(const_reference data) { emplace_front(data); }
    void push_back(T&& data) { emplace_back(std::move(data)); }
    void push_front(T&& data) { emplace_front(std::move(data)); }
    iterator insert(iterator pos, const_reference data) { return emplace(pos, data); }
    iterator insert(iterator pos, T&& data) { return emplace(pos, std::move(data)); }

    void pop_back() noexcept {
        Node* p = sentinel_.prev_;
        p->prev_->next_ = &sentinel_;
        sentinel_.prev_ = p->prev_;

        destroy_node(p);
        --size_;
    }
    void pop_front() noexcept {
        Node* p = sentinel_.next_;
        sentinel_.next_ = p->next_;
        p->next_->prev_ = &sentinel_;

        destroy_node(p);
        --size_;
    }

    iterator erase(iterator pos) noexcept {
        if (pos.curr_ == &sentinel_) {
            return end();
        }

        Node* p = pos.curr_;
        iterator next_it(p->next_);

        p->prev_->next_ = p->next_;
        p->next_->prev_ = p->prev_;

        destroy_node(p);
        --size_;
        return next_it;
    }

    template<typename Compare = std::less<value_type>>
    void sort(Compare comp) noexcept {
        if (size_ <= 1) {
            return;
        }

        Node* new_head = merge_sort<Compare>(sentinel_.next_, comp);

        sentinel_.next_ = new_head;
        if (new_head) {
            new_head->prev_ = &sentinel_;

            Node* curr = new_head;
            while (curr->next_ != &sentinel_) {
                curr = curr->next_;
            }
            sentinel_.prev_ = curr;
            curr->next_ = &sentinel_;
        }
        else {
            sentinel_.next_ = sentinel_.prev_ = &sentinel_;
        }
    }
    void sort() noexcept { sort(std::less<value_type>{}); }

    template<typename Compare = std::less<value_type>>
    void merge(List&& other, Compare comp) noexcept {
        if (this == &other) return;
        if (!std::is_sorted(begin(), end(), comp) || 
            !std::is_sorted(other.begin(), other.end(), comp)) return;

        Node* this_curr = sentinel_.next_;
        Node* other_curr = other.sentinel_.next_;
        while (this_curr != &sentinel_ && other_curr != &other.sentinel_) {
            if (comp(other_curr->data_, this_curr->data_)) {
                Node* next_other = other_curr->next_;
                other_curr->prev_ = this_curr->prev_;
                other_curr->next_ = this_curr;
                this_curr->prev_->next_ = other_curr;
                this_curr->prev_ = other_curr;
                other_curr = next_other;
                ++size_;
                --other.size_;
            } else {
                this_curr = this_curr->next_;
            }
        }
        while (other_curr != &other.sentinel_) {
            Node* next_other = other_curr->next_;
            other_curr->prev_ = sentinel_.prev_;
            other_curr->next_ = &sentinel_;
            sentinel_.prev_->next_ = other_curr;
            sentinel_.prev_ = other_curr;
            other_curr = next_other;
            ++size_;
            --other.size_;
        }
        other.sentinel_.next_ = &other.sentinel_;
        other.sentinel_.prev_ = &other.sentinel_;
    }
    void merge(List&& other) noexcept { merge(std::move(other), std::less<value_type>{}); }

private:
    struct Node {
        value_type data_;
        Node* next_;
        Node* prev_;

        Node() : data_(), next_(this), prev_(this) {}

        template<typename... Args>
        Node(Args&&... args) : data_(std::forward<Args>(args)...), next_(nullptr), prev_(nullptr) {}
    };
    size_type size_;
    Node sentinel_;

    template<typename... Args>
    Node* create_node(Args&&... args) {
        return new Node(std::forward<Args>(args)...);
    }

    void destroy_node(Node* p) noexcept {
        delete p;
    }   
    
    template <typename Compare = std::less<T>>
    Node* merge_sort(Node* head, Compare comp) noexcept {
        if (!head || head->next_ == &sentinel_) {
            return head;
        }

        Node* slow = head;
        Node* fast = head->next_;
        while (fast != &sentinel_ && fast->next_ != &sentinel_) {
            slow = slow->next_;
            fast = fast->next_->next_;
        }

        Node* mid = slow->next_;
        slow->next_ = &sentinel_;
        if (mid != &sentinel_) {
            mid->prev_ = nullptr;
        }

        Node* left = merge_sort(head, comp);
        Node* right = merge_sort(mid, comp);

        if (!left) return right;
        if (!right) return left;

        Node* res = nullptr;
        if (comp(left->data_, right->data_)) {
            res = left;
            left = left->next_;
        } else {
            res = right;
            right = right->next_;
        }

        Node* tail = res;
        tail->next_ = nullptr;
        tail->prev_ = nullptr;

        while (left != &sentinel_ && right != &sentinel_) {
            if (comp(left->data_, right->data_)) {
                tail->next_ = left;
                left->prev_ = tail;
                left = left->next_;
            } else {
                tail->next_ = right;
                right->prev_ = tail;
                right = right->next_;
            }
            tail = tail->next_;
            tail->next_ = nullptr;
        }

        if (left != &sentinel_) {
            tail->next_ = left;
            left->prev_ = tail;
            while (tail->next_ != &sentinel_) {
                tail = tail->next_;
            }
        } else if (right != &sentinel_) {
            tail->next_ = right;
            right->prev_ = tail;
            while (tail->next_ != &sentinel_) {
                tail = tail->next_;
            }
        }

        return res;
    }
};

template <typename T>
void swap(List<T>& lhs, List<T>& rhs) noexcept {
    lhs.swap(rhs);
}

}