#pragma once
#include "containers/vector.hpp"
#include <optional>
#include <utility>
#include <limits>

namespace tables {

template <typename Key, typename T>
class UnsortedArrayTable {
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

    using iterator = typename containers::Vector<value_type>::iterator;
    using const_iterator = typename containers::Vector<value_type>::const_iterator;

    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_impl(value.first, value.second);
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_impl(std::move(value.first), std::move(value.second));
    }

    iterator find(const key_type& key) {
        return find_impl(data_.begin(), data_.end(), key);
    }

    const_iterator find(const key_type& key) const {
        return find_impl(data_.begin(), data_.end(), key);
    }

    iterator erase(iterator pos) {
        return iterator(data_.erase(pos));
    }

    iterator erase(const_iterator pos) {
        return iterator(data_.erase(pos));
    }

    iterator erase(const key_type& key) {
        auto it = find(key);
        if (it != end()) {
            std::swap(*it, data_.back());
            data_.pop_back();
            return it;
        }
        return end();
    }

    void reserve(size_type new_cap) {
        data_.reserve(new_cap);
    }

	std::string type_name() const noexcept { return "UnsortedArrayTable"; }
    size_type op_count() const noexcept { return count_; }
    void reset_op_count() noexcept { count_ = 0; }
    bool empty() const noexcept { return data_.empty(); }
    size_type size() const noexcept { return data_.size(); }
    void clear() noexcept { data_.clear(); }

    iterator begin() noexcept { return iterator(data_.begin()); }
    iterator end() noexcept { return iterator(data_.end()); }
    const_iterator begin() const noexcept { return const_iterator(data_.begin()); }
    const_iterator end() const noexcept { return const_iterator(data_.end()); }
    const_iterator cbegin() const noexcept { return const_iterator(data_.cbegin()); }
    const_iterator cend() const noexcept { return const_iterator(data_.cend()); }

private:
    containers::Vector<value_type> data_;
    mutable size_type count_ = 0;

    template <typename Iter>
    Iter find_impl(Iter first, Iter last, const key_type& key) const {
        auto it = std::find_if(first, last, [this, &key](const value_type& pair) {
            ++count_;
            return pair.first == key;
        });
        return it;
    }

    template<typename K, typename V>
    std::pair<iterator, bool> insert_impl(K&& key, V&& value) {
        auto it = find(key);
		if (it != end()) {
            it->second = std::forward<V>(value);
            return { it, false };
        }
        data_.push_back(value_type(std::forward<K>(key), std::forward<V>(value)));
        return { iterator(data_.end() - 1), true };
    }

};


}