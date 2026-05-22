#pragma once
#include "containers/list.hpp"
#include <initializer_list>
#include <type_traits>

namespace containers {

template <typename T, typename Compare = std::less<T>>
class SortedList {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using iterator = typename List<T>::iterator;
    using const_iterator = typename List<T>::const_iterator;

    SortedList() = default;

    template<std::input_iterator Iter>
    SortedList(Iter first, Iter last) {
        std::copy(first, last, std::back_inserter(list_));
        list_.sort(comp_);
    }

    SortedList(std::initializer_list<value_type> init) : SortedList(init.begin(), init.end()) {}

    void clear() noexcept { list_.clear(); }
    bool empty() const noexcept { return list_.empty(); }
    size_type size()  const noexcept { return list_.size(); }
    reference front() { return list_.front(); }
    const_reference front() const { return list_.front(); }
    reference back() { return list_.back(); }
    const_reference back()  const { return list_.back(); }

    iterator insert(const_reference data) {
        auto pos = std::lower_bound(begin(), end(), data, comp_);
        return list_.insert(pos, data);
    }

    iterator insert(T&& data) {
        auto pos = std::lower_bound(begin(), end(), data, comp_);
        return list_.insert(pos, std::move(data));
    }

    iterator insert_unique(const_reference data) {
        auto pos = std::lower_bound(begin(), end(), data, comp_);
        if (pos != end() && !comp_(data, *pos) && !comp_(*pos, data)) {
            return pos;
        }
        return list_.insert(pos, data);
    }

    iterator insert_unique(T&& data) {
        auto pos = std::lower_bound(begin(), end(), data, comp_);
        if (pos != end() && !comp_(data, *pos) && !comp_(*pos, data)) {
            return pos;
        }
        return list_.insert(pos, std::move(data));
    }

    iterator find(const_reference key) noexcept {
        auto it = std::lower_bound(begin(), end(), key, comp_);
        if (it != end() && !comp_(key, *it) && !comp_(*it, key)) {
            return it;
        }
        return end();
    }

    const_iterator find(const_reference key) const noexcept {
        auto it = std::lower_bound(begin(), end(), key, comp_);
        if (it != end() && !comp_(key, *it) && !comp_(*it, key)) {
            return it;
        }
        return end();
    }

    iterator erase(iterator pos) noexcept {
        return list_.erase(pos);
    }

    void merge(SortedList&& other) noexcept {
        if (this == &other || other.empty()) return;
        list_.merge(std::move(other.list_), comp_);
    }

    iterator       begin()         noexcept { return list_.begin(); }
    const_iterator begin()   const noexcept { return list_.begin(); }
    iterator       end()           noexcept { return list_.end(); }
    const_iterator end()     const noexcept { return list_.end(); }
    const_iterator cbegin()  const noexcept { return list_.cbegin(); }
    const_iterator cend()    const noexcept { return list_.cend(); }

    void swap(SortedList& rhs) noexcept {
        using std::swap;
        swap(list_, rhs.list_);
        swap(comp_, rhs.comp_);
    }

private:
    List<value_type> list_;
    Compare comp_;
};

template <typename T, typename Compare>
void swap(SortedList<T, Compare>& lhs, SortedList<T, Compare>& rhs) noexcept {
    lhs.swap(rhs);
}

}