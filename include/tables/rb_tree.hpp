#pragma once
#include "containers/stack.hpp"
#include <optional>
#include <utility>
#include <limits>

namespace tables {

template <typename Key,
    typename T,
    typename Compare = std::less<Key>>
class RBTreeTable {
    struct Node;
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<Key, T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using key_compare = Compare;

    RBTreeTable() { init_nil(); }

    RBTreeTable(const key_compare& comp)
        : comp_(comp) {
        init_nil();
    }

    ~RBTreeTable() {
        clear();
        delete nil_;
    }

    RBTreeTable(const RBTreeTable& other)
        : comp_(other.comp_) {
        if (other.root_ != other.nil_) {
            root_ = copy_subtree(other.root_, other.nil_, nil_);
            size_ = other.size_;
        }
        else {
            root_ = nil_;
        }
    }

    RBTreeTable& operator=(const RBTreeTable& other) {
        if (this == &other) return *this;
        RBTreeTable tmp(other);
        swap(tmp);
        return *this;
    }

    RBTreeTable(RBTreeTable&& other) noexcept
        : root_(other.root_)
        , nil_(other.nil_)
        , comp_(std::move(other.comp_))
        , size_(other.size_)
        , count_(other.count_) {
        other.root_ = nullptr;
        other.nil_ = nullptr;
        other.size_ = 0;
        other.count_ = 0;
    }

    RBTreeTable& operator=(RBTreeTable&& other) noexcept {
        if (this == &other) return *this;
        RBTreeTable tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    void swap(RBTreeTable& other) noexcept {
        std::swap(root_, other.root_);
        std::swap(nil_, other.nil_);
        std::swap(size_, other.size_);
        std::swap(comp_, other.comp_);
        std::swap(count_, other.count_);
    }

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename RBTreeTable::value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator(Node* node = nullptr, Node* nil = nullptr)
            : node_(node), nil_(nil) {
        }

        reference operator*() const noexcept { return node_->data_; }
        pointer operator->() const noexcept { return &(node_->data_); }

        bool operator==(const Iterator& other) const noexcept {
            return node_ == other.node_;
        }

        bool operator!=(const Iterator& other) const noexcept {
            return node_ != other.node_;
        }

        Iterator& operator++() noexcept {
            if (node_ == nil_) return *this;

            if (node_->right_ != nil_) {
                node_ = min(node_->right_);
            }
            else {
                Node* parent = node_->parent_;
                while (parent != nil_ && node_ == parent->right_) {
                    node_ = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

    private:
        Node* node_;
        Node* nil_;

        Node* min(Node* node) noexcept {
            while (node->left_ != nil_) {
                node = node->left_;
            }
            return node;
        }
        friend class RBTreeTable;
    };

    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const typename RBTreeTable::value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        ConstIterator(const Node* node = nullptr, const Node* nil = nullptr)
            : node_(node), nil_(nil) {
        }

        ConstIterator(const Iterator& it)
            : node_(it.node_), nil_(it.nil_) {
        }

        reference operator*() const noexcept { return node_->data_; }
        pointer operator->() const noexcept { return &(node_->data_); }

        bool operator==(const ConstIterator& other) const noexcept {
            return node_ == other.node_;
        }

        bool operator!=(const ConstIterator& other) const noexcept {
            return node_ != other.node_;
        }

        ConstIterator& operator++() noexcept {
            if (node_ == nil_) return *this;

            if (node_->right_ != nil_) {
                node_ = min(node_->right_);
            }
            else {
                const Node* parent = node_->parent_;
                while (parent != nil_ && node_ == parent->right_) {
                    node_ = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }
            return *this;
        }

        ConstIterator operator++(int) noexcept {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

    private:
        const Node* node_;
        const Node* nil_;

        const Node* min(const Node* node) const noexcept {
            while (node->left_ != nil_) {
                node = node->left_;
            }
            return node;
        }
        friend class RBTreeTable;
    };

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_impl(value.first, value.second);
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_impl(std::move(value.first), std::move(value.second));
    }

    iterator erase(iterator pos) noexcept {
        return erase_impl(pos);
    }

    iterator erase(const_iterator pos) noexcept {
        return erase_impl(pos);
	}

    iterator erase(const key_type& key) noexcept {
        auto it = find(key);
        if (it != end()) return erase(it);
        return end();
	}

    iterator find(const key_type& key) noexcept {
        return find_impl<iterator>(root_, nil_, key);
    }

    const_iterator find(const key_type& key) const noexcept {
        return find_impl<const_iterator>(root_, nil_, key);
    }

    size_type op_count() const noexcept { return count_; }
    void reset_op_count() noexcept { count_ = 0; }
    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept {
        if (is_nil(root_)) return;

        containers::Stack<Node*> stack;
        stack.push(root_);

        while (!stack.empty()) {
            Node* node = stack.top();
            stack.pop();

            if (!is_nil(node->left_)) stack.push(node->left_);
            if (!is_nil(node->right_)) stack.push(node->right_);

            delete node;
        }

        root_ = nil_;
        size_ = 0;
    }

    iterator begin() noexcept { return iterator(min(root_), nil_); }
    iterator end() noexcept { return iterator(nil_, nil_); }
    const_iterator begin() const noexcept { return const_iterator(min(root_), nil_); }
    const_iterator end() const noexcept { return const_iterator(nil_, nil_); }
    const_iterator cbegin() const noexcept { return const_iterator(min(root_), nil_); }
    const_iterator cend() const noexcept { return const_iterator(nil_, nil_); }

private:
    enum class Color : uint8_t { RED, BLACK };

    struct Node {
        value_type data_;
        Node* left_;
        Node* right_;
        Node* parent_;
        Color color_;

        template<typename K, typename V>
        Node(K&& key, V&& value, Color c = Color::RED)
            : data_(std::forward<K>(key), std::forward<V>(value))
            , left_(nullptr)
            , right_(nullptr)
            , parent_(nullptr)
            , color_(c) {
        }

        bool is_left_child() const noexcept {
            return parent_ && this == parent_->left_;
        }

        bool is_right_child() const noexcept {
            return parent_ && this == parent_->right_;
        }

        Node* grandparent() const noexcept {
            return parent_ ? parent_->parent_ : nullptr;
        }

        Node* uncle() const noexcept {
            Node* gp = grandparent();
            if (!gp) return nullptr;
            return parent_->is_left_child() ? gp->right_ : gp->left_;
        }

        Node* sibling() const noexcept {
            if (!parent_) return nullptr;
            return is_left_child() ? parent_->right_ : parent_->left_;
        }
    };

    static bool is_red(Node* node) noexcept {
        return node && node->color_ == Color::RED;
    }

    static bool is_black(Node* node) noexcept {
        return !node || node->color_ == Color::BLACK;
    }

    static void set_red(Node* node) noexcept {
        if (node) node->color_ = Color::RED;
    }

    static void set_black(Node* node) noexcept {
        if (node) node->color_ = Color::BLACK;
    }

    static void set_color(Node* node, Color color) noexcept {
        if (node) node->color_ = color;
    }

    Node* root_ = nullptr;
    Node* nil_ = nullptr;
    key_compare comp_{};
    size_type size_ = 0;
    mutable size_type count_ = 0;

    void init_nil() {
        nil_ = new Node(key_type(), mapped_type(), Color::BLACK);
        nil_->left_ = nil_;
        nil_->right_ = nil_;
        nil_->parent_ = nil_;
        root_ = nil_;
    }

    bool is_nil(Node* node) const noexcept { return node == nil_; }

    template<typename K, typename V>
    Node* create_node(K&& key, V&& value, Color color = Color::RED) {
        Node* node = new Node(std::forward<K>(key), std::forward<V>(value), color);
        node->left_ = nil_;
        node->right_ = nil_;
        node->parent_ = nil_;
        return node;
    }

    Node* min(Node* node) const noexcept {
        if (is_nil(node)) return node;
        while (!is_nil(node->left_)) {
            ++count_;
            node = node->left_;
        }
        return node;
    }

    void left_rotate(Node* x) noexcept {
        ++count_;
        Node* y = x->right_;

        x->right_ = y->left_;
        if (!is_nil(y->left_)) y->left_->parent_ = x;

        y->parent_ = x->parent_;

        if (is_nil(x->parent_)) root_ = y;
        else if (x == x->parent_->left_) x->parent_->left_ = y;
        else x->parent_->right_ = y;

        y->left_ = x;
        x->parent_ = y;
    }

    void right_rotate(Node* y) noexcept {
        ++count_;
        Node* x = y->left_;

        y->left_ = x->right_;
        if (!is_nil(x->right_)) x->right_->parent_ = y;

        x->parent_ = y->parent_;

        if (is_nil(y->parent_)) root_ = x;
        else if (y == y->parent_->right_) y->parent_->right_ = x;
        else y->parent_->left_ = x;

        x->right_ = y;
        y->parent_ = x;
    }

    void insert_fix(Node* z) noexcept {
        while (is_red(z->parent_)) {
            ++count_;
            if (z->parent_ == z->grandparent()->left_) {
                Node* y = z->grandparent()->right_; 

                if (is_red(y)) {
                    set_black(z->parent_);
                    set_black(y);
                    set_red(z->grandparent());
                    z = z->grandparent();
                } else {
                    if (z == z->parent_->right_) {
                        z = z->parent_;
                        left_rotate(z);
                    }
                    set_black(z->parent_);
                    set_red(z->grandparent());
                    right_rotate(z->grandparent());
                }
            } else {
                Node* y = z->grandparent()->left_; 

                if (is_red(y)) {
                    set_black(z->parent_);
                    set_black(y);
                    set_red(z->grandparent());
                    z = z->grandparent();
                } else {
                    if (z == z->parent_->left_) {
                        z = z->parent_;
                        right_rotate(z);
                    }
                    set_black(z->parent_);
                    set_red(z->grandparent());
                    left_rotate(z->grandparent());
                }
            }
        }
        set_black(root_);
    }

    void remove_fix(Node* x) noexcept {
        while (x != root_ && is_black(x)) {
            ++count_;
            if (x == x->parent_->left_) {
                Node* w = x->parent_->right_; 

                if (is_red(w)) {
                    set_black(w);
                    set_red(x->parent_);
                    left_rotate(x->parent_);
                    w = x->parent_->right_;
                }

                if (is_black(w->left_) && is_black(w->right_)) {
                    set_red(w);
                    x = x->parent_;
                } else {
                    if (is_black(w->right_)) {
                        set_black(w->left_);
                        set_red(w);
                        right_rotate(w);
                        w = x->parent_->right_;
                    }
                    set_color(w, x->parent_->color_);
                    set_black(x->parent_);
                    set_black(w->right_);
                    left_rotate(x->parent_);
                    x = root_;
                }
            } else {
                Node* w = x->parent_->left_;

                if (is_red(w)) {
                    set_black(w);
                    set_red(x->parent_);
                    right_rotate(x->parent_);
                    w = x->parent_->left_;
                }

                if (is_black(w->right_) && is_black(w->left_)) {
                    set_red(w);
                    x = x->parent_;
                } else {
                    if (is_black(w->left_)) {
                        set_black(w->right_);
                        set_red(w);
                        left_rotate(w);
                        w = x->parent_->left_;
                    }
                    set_color(w, x->parent_->color_);
                    set_black(x->parent_);
                    set_black(w->left_);
                    right_rotate(x->parent_);
                    x = root_;
                }
            }
        }
        set_black(x);
    }

    void transplant(Node* u, Node* v) noexcept {
        ++count_;
        if (is_nil(u->parent_)) root_ = v;
        else if (u == u->parent_->left_) u->parent_->left_ = v;
        else u->parent_->right_ = v;

        v->parent_ = u->parent_;
    }

    void erase_node(Node* z) noexcept {
        ++count_;
        if (is_nil(z)) return;

        Node* y = z;
        Node* x;
        Color y_original_color = y->color_;

        if (is_nil(z->left_)) {
            x = z->right_;
            transplant(z, z->right_);
        } else if (is_nil(z->right_)) {
            x = z->left_;
            transplant(z, z->left_);
        } else {
            y = min(z->right_);
            y_original_color = y->color_;
            x = y->right_;

            if (y->parent_ == z) {
                x->parent_ = y;
            } else {
                transplant(y, y->right_);
                y->right_ = z->right_;
                y->right_->parent_ = y;
            }

            transplant(z, y);
            y->left_ = z->left_;
            y->left_->parent_ = y;
            set_color(y, z->color_);  
        }

        delete z;
        --size_;

        if (y_original_color == Color::BLACK) {
            remove_fix(x);
        }
    }

    template <typename Iter>
    Iter find_impl(Node* begin, Node* end, const key_type& key) const noexcept {
        Node* curr = begin;
        while (curr != end) {
            ++count_;
            if (comp_(key, curr->data_.first)) {
                curr = curr->left_;
            } else if (comp_(curr->data_.first, key)) {
                curr = curr->right_;
            } else {
                return Iter(curr, nil_);
            }
        }
        return Iter(end, nil_);
    }

    template <typename Iter>
	Iter erase_impl(Iter pos) noexcept {
        if (pos == end()) return end();
        Node* node = pos.node_;
        Iter next = pos;
        ++next;
        erase_node(node);
        return next;
    }

    template <typename K, typename V>
    std::pair<iterator, bool> insert_impl(K&& key, V&& value) {
        Node* parent = nil_;
        Node* curr = root_;

        while (!is_nil(curr)) {
            ++count_;
            parent = curr;

            if (comp_(key, curr->data_.first)) {
                curr = curr->left_;
            } else if (comp_(curr->data_.first, key)) {
                curr = curr->right_;
            } else {
                curr->data_.second = std::forward<V>(value);
                return { iterator(curr, nil_), false };
            }
        }

        Node* new_node = create_node(
            std::forward<K>(key),
            std::forward<V>(value)
        );
        new_node->parent_ = parent;

        if (is_nil(parent)) {
            root_ = new_node;
        } else if (comp_(new_node->data_.first, parent->data_.first)) {
            parent->left_ = new_node;
        } else {
            parent->right_ = new_node;
        }

        insert_fix(new_node);
        ++size_;
        return { iterator(new_node, nil_), true };
    }

    Node* copy_subtree(Node* other_node, Node* other_nil, Node* my_nil) {
		if (other_node == other_nil) return my_nil;
            
        containers::Stack<std::pair<Node*, Node*>> stack;

        Node* new_root = create_node(
            other_node->data_.first, 
            other_node->data_.second, 
            other_node->color_);
        new_root->parent_ = my_nil;
        stack.push({ other_node, new_root });

        while(!stack.empty()) {
            auto [other_curr, new_curr] = stack.top(); stack.pop();
            if (other_curr->left_ != other_nil) {
                Node* left_child = create_node(
                    other_curr->left_->data_.first,
                    other_curr->left_->data_.second,
                    other_curr->left_->color_);
                left_child->parent_ = new_curr;
                new_curr->left_ = left_child;
                stack.push({ other_curr->left_, left_child });
            } else {
                new_curr->left_ = my_nil;
            }
            if (other_curr->right_ != other_nil) {
                Node* right_child = create_node(
                    other_curr->right_->data_.first,
                    other_curr->right_->data_.second,
                    other_curr->right_->color_);
                right_child->parent_ = new_curr;
                new_curr->right_ = right_child;
                stack.push({ other_curr->right_, right_child });
            } else {
                new_curr->right_ = my_nil;
            }
        }
        return new_root;

    }

};

}