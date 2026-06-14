#pragma once
#include "containers/stack.hpp"
#include <optional>
#include <utility>
#include <limits>
#include <concepts>

namespace tables {

template <typename Key,
    typename T,
    typename Compare = std::less<Key>>
class RBTreeTable {
    struct Node;
	using node_ptr = Node*;
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using key_compare = Compare;

    RBTreeTable() { init_nil(); }

    ~RBTreeTable() {
        clear();
        delete nil_;
    }

    RBTreeTable(const RBTreeTable& other)
        : comp_(other.comp_) {
        if (other.root_ != other.nil_) {
            root_ = copy_subtree(other.root_, other.nil_, nil_);
            size_ = other.size_;
        } else {
            root_ = nil_;
        }
    }

    RBTreeTable& operator=(const RBTreeTable& other) {
        RBTreeTable tmp(other);
        swap(tmp);
        return *this;
    }

    RBTreeTable(RBTreeTable&& other) noexcept
        : root_(std::exchange(other.root_, nullptr)),
        nil_(std::exchange(other.nil_, nullptr)),
        comp_(std::move(other.comp_)),
        size_(std::exchange(other.size_, 0)),
        count_(std::exchange(other.count_, 0)) {}

    RBTreeTable& operator=(RBTreeTable&& other) noexcept {
        RBTreeTable tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    void swap(RBTreeTable& other) noexcept {
        using std::swap;
        swap(root_, other.root_);
        swap(nil_, other.nil_);
        swap(size_, other.size_);
        swap(comp_, other.comp_);
        swap(count_, other.count_);
    }

	template <typename U>
    class RBIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = U;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
		using node_ptr = std::conditional_t<std::is_const_v<U>, const Node*, Node*>;

        RBIterator(node_ptr node = nullptr, node_ptr nil = nullptr)
            : node_(node), nil_(nil) {
        }

		template <typename V>
		requires std::same_as<std::remove_const_t<U>, std::remove_const_t<V>>
		RBIterator(const RBIterator<V>& other) noexcept
			: node_(other.node_), nil_(other.nil_) {
		}

        reference operator*() const noexcept { return node_->data_; }
        pointer operator->() const noexcept { return &(node_->data_); }

        bool operator==(const RBIterator& other) const noexcept {
            return node_ == other.node_;
        }

        bool operator!=(const RBIterator& other) const noexcept {
            return node_ != other.node_;
        }

        RBIterator& operator++() noexcept {
            if (node_ == nil_) return *this;

            if (node_->right_ != nil_) {
                node_ = min(node_->right_);
            } else {
                node_ptr parent = node_->parent_;
                while (parent != nil_ && node_ == parent->right_) {
                    node_ = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }
            return *this;
        }

        RBIterator operator++(int) noexcept {
            RBIterator tmp = *this;
            ++(*this);
            return tmp;
        }

    private:
        node_ptr node_;
        node_ptr nil_;

        node_ptr min(node_ptr node) noexcept {
            while (node->left_ != nil_) {
                node = node->left_;
            }
            return node;
        }
        friend class RBTreeTable<Key, T, Compare>;
        template <typename V>
        friend class ConstIterator;
    };

    using iterator = RBIterator<value_type>;
    using const_iterator = RBIterator<const value_type>;

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

    std::string type_name() const noexcept { return "RBTreeTable"; }
    size_type op_count() const noexcept { return count_; }
    void reset_op_count() noexcept { count_ = 0; }
    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    void clear() noexcept {
        clear_impl(root_);
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
        node_ptr left_;
        node_ptr right_;
        node_ptr parent_;
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

        node_ptr grandparent() const noexcept {
            return parent_ ? parent_->parent_ : nullptr;
        }

        node_ptr uncle() const noexcept {
            auto gp = grandparent();
            if (!gp) return nullptr;
            return parent_->is_left_child() ? gp->right_ : gp->left_;
        }

        node_ptr sibling() const noexcept {
            if (!parent_) return nullptr;
            return is_left_child() ? parent_->right_ : parent_->left_;
        }
    };

    static bool is_red(node_ptr node) noexcept {
        return node && node->color_ == Color::RED;
    }

    static bool is_black(node_ptr node) noexcept {
        return !node || node->color_ == Color::BLACK;
    }

    static void set_red(node_ptr node) noexcept {
        if (node) node->color_ = Color::RED;
    }

    static void set_black(node_ptr node) noexcept {
        if (node) node->color_ = Color::BLACK;
    }

    static void set_color(node_ptr node, Color color) noexcept {
        if (node) node->color_ = color;
    }

    node_ptr root_ = nullptr;
    node_ptr nil_ = nullptr;
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

    bool is_nil(node_ptr node) const noexcept { return node == nil_; }

    template<typename K, typename V>
    node_ptr create_node(K&& key, V&& value, Color color = Color::RED) {
        auto node = new Node(std::forward<K>(key), std::forward<V>(value), color);
        node->left_ = nil_;
        node->right_ = nil_;
        node->parent_ = nil_;
        return node;
    }

    node_ptr min(node_ptr node) const noexcept {
        if (is_nil(node)) return node;
        while (!is_nil(node->left_)) {
            ++count_;
            node = node->left_;
        }
        return node;
    }

    void left_rotate(node_ptr x) noexcept {
        ++count_;
        auto y = x->right_;

        x->right_ = y->left_;
        if (!is_nil(y->left_)) y->left_->parent_ = x;

        y->parent_ = x->parent_;

        if (is_nil(x->parent_)) root_ = y;
        else if (x == x->parent_->left_) x->parent_->left_ = y;
        else x->parent_->right_ = y;

        y->left_ = x;
        x->parent_ = y;
    }

    void right_rotate(node_ptr y) noexcept {
        ++count_;
        auto x = y->left_;

        y->left_ = x->right_;
        if (!is_nil(x->right_)) x->right_->parent_ = y;

        x->parent_ = y->parent_;

        if (is_nil(y->parent_)) root_ = x;
        else if (y == y->parent_->right_) y->parent_->right_ = x;
        else y->parent_->left_ = x;

        x->right_ = y;
        y->parent_ = x;
    }

    void insert_fix(node_ptr z) noexcept {
        while (is_red(z->parent_)) {
            ++count_;
            if (z->parent_ == z->grandparent()->left_) {
                auto y = z->grandparent()->right_;

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
                auto y = z->grandparent()->left_;

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

    void remove_fix(node_ptr x) noexcept {
        while (x != root_ && is_black(x)) {
            ++count_;
            if (x == x->parent_->left_) {
                auto w = x->parent_->right_;

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
                auto w = x->parent_->left_;

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

    void transplant(node_ptr u, node_ptr v) noexcept {
        ++count_;
        if (is_nil(u->parent_)) root_ = v;
        else if (u == u->parent_->left_) u->parent_->left_ = v;
        else u->parent_->right_ = v;

        v->parent_ = u->parent_;
    }

    void erase_node(node_ptr z) noexcept {
        ++count_;
        if (is_nil(z)) return;

        auto y = z;
        node_ptr x;
        auto y_original_color = y->color_;

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
    Iter find_impl(node_ptr begin, node_ptr end, const key_type& key) const noexcept {
        auto curr = begin;
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
        auto node = pos.node_;
        auto next = pos;
        ++next;
        erase_node(node);
        return next;
    }

    template <typename K, typename V>
    std::pair<iterator, bool> insert_impl(K&& key, V&& value) {
        auto parent = nil_;
        auto curr = root_;

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

        auto new_node = create_node(
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

    node_ptr copy_subtree(node_ptr other_node, node_ptr other_nil, node_ptr my_nil) {
        if (other_node == other_nil) return my_nil;

        auto new_node = create_node(
            other_node->data_.first,
            other_node->data_.second,
            other_node->color_
        );

        new_node->left_ = copy_subtree(other_node->left_, other_nil, my_nil);
        new_node->right_ = copy_subtree(other_node->right_, other_nil, my_nil);

        if (new_node->left_ != my_nil) new_node->left_->parent_ = new_node;
        if (new_node->right_ != my_nil) new_node->right_->parent_ = new_node;

        return new_node;
    }

    void clear_impl(node_ptr node) noexcept {
        if (is_nil(node)) return;
        clear_impl(node->left_);
        clear_impl(node->right_);
        delete node;
    }

};

}