#pragma once
#include "containers/vector.hpp"
#include <optional>
#include <limits>
#include <random>

namespace tables {

template <typename Key,
          typename T,
          typename Hasher = std::hash<Key>,
          typename Equal = std::equal_to<Key>>
class HashTable {
    struct Bucket;
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

    HashTable(size_type init_capacity = 16)
        : buckets_(next_prime(init_capacity))
        , size_(0)
        , seed_(std::random_device{}()) {}

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = HashTable::value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        Iterator(HashTable* ptr = nullptr, size_type index = 0)
            : table_(ptr), index_(index) {
            if (table_ && index_ < table_->buckets_.size() &&
                !table_->buckets_[index_].is_occupied()) {
                skip();
            }
        }

        reference operator*() const noexcept {
            return table_->buckets_[index_].value_;
        }
        pointer operator->() const noexcept {
            return &table_->buckets_[index_].value_;
        }

        Iterator& operator++() noexcept {
            ++index_;
            skip();
            return *this;
        }
        Iterator operator++(int) noexcept {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const noexcept {
            return table_ == other.table_ && index_ == other.index_;
        }
        bool operator!=(const Iterator& other) const noexcept {
            return !(*this == other);
        }
        

    private:
        void skip() noexcept {
            while (index_ < table_->buckets_.size() &&
                !table_->buckets_[index_].is_occupied()) {
                ++index_;
            }
        }

        HashTable* table_;
        size_type index_;
        friend class HashTable;
    };

    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const HashTable::value_type;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;
        
        ConstIterator(const HashTable* ptr = nullptr, size_type index = 0)
            : table_(ptr), index_(index) {
            if (table_ && index_ < table_->buckets_.size() &&
                !table_->buckets_[index_].is_occupied()) {
                skip();
            }
        }

        ConstIterator(const Iterator& it) : table_(it.table_), index_(it.index_) {}

        reference operator*() const noexcept {
            return table_->buckets_[index_].value_;
        }
        pointer operator->() const noexcept {
            return &table_->buckets_[index_].value_;
        }

        ConstIterator& operator++() noexcept {
            ++index_;
            skip();
            return *this;
        }
        ConstIterator operator++(int) noexcept {
            ConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const ConstIterator& other) const noexcept {
            return table_ == other.table_ && index_ == other.index_;
        }
        bool operator!=(const ConstIterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        void skip() noexcept {
            while (index_ < table_->buckets_.size() &&
                !table_->buckets_[index_].is_occupied()) {
                ++index_;
            }
        }
        const HashTable* table_;
        size_type index_;
        friend class HashTable;
    };

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_impl(value.first, value.second);
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_impl(std::move(value.first), std::move(value.second));
    }

    iterator erase(const key_type& key) {
        auto pos = find_pos(key);
        if (pos.has_value() && buckets_[*pos].is_occupied()) {
            buckets_[*pos].set_deleted();
            --size_;
            return iterator(this, *pos);
        }
        return end();
    }

    iterator erase(iterator pos) {
        if (pos == end() || pos.table_ != this) {
            return end();
        }

        size_type current_index = pos.index_;

        if (buckets_[current_index].is_occupied()) {
            buckets_[current_index].set_deleted();
            --size_;
        }
        ++pos; 
        return pos;
    }

    iterator erase(const_iterator pos) {
        if (pos == end() || pos.table_ != this) {
            return end();
        }

        size_type current_index = pos.index_;

        if (buckets_[current_index].is_occupied()) {
            buckets_[current_index].set_deleted();
            --size_;
        }
        iterator next_it(this, current_index);
        ++next_it;
        return next_it;
    }

    iterator find(const key_type& key) {
        auto pos = find_pos(key);
        if (pos.has_value() && buckets_[*pos].is_occupied()) {
            return iterator(this, *pos);
        }
        return end();
    }

    const_iterator find(const key_type& key) const {
        auto pos = find_pos(key);
        if (pos.has_value() && buckets_[*pos].is_occupied()) {
            return const_iterator(this, *pos);
        }
        return end();
    }

    void reserve(size_type new_cap) {
        if (new_cap > buckets_.size()) {
            rehash(next_prime(new_cap));
        }
    }

    float load_factor() const noexcept {
        return static_cast<float>(size_) / static_cast<float>(buckets_.size());
    }
    void max_load_factor(float new_load_factor) {
        if (new_load_factor <= 0.0f || new_load_factor >= 1.0f) {
            throw std::invalid_argument("Load factor must be in (0, 1)");
        }
        load_factor_ = new_load_factor;
    }
    float max_load_factor() const noexcept { return load_factor_; }
    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    size_type op_count() const noexcept { return count_; }
    void reset_op_count() noexcept { count_ = 0; }
    void clear() noexcept {
        for (auto& bucket : buckets_) {
            bucket.set_empty();
        }
        size_ = 0;
    }

    iterator begin() noexcept { return iterator(this, 0); }
    iterator end() noexcept { return iterator(this, buckets_.size()); }
    const_iterator begin() const noexcept { return const_iterator(this, 0); }
    const_iterator end() const noexcept { return const_iterator(this, buckets_.size()); }
    const_iterator cbegin() const noexcept { return const_iterator(this, 0); }
    const_iterator cend() const noexcept { return const_iterator(this, buckets_.size()); }

private:
    enum class BucketStatus : uint8_t {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    struct Bucket {
        value_type value_;
        BucketStatus status_;

        Bucket() : value_(), status_(BucketStatus::EMPTY) {}
        template<typename K, typename V>
        Bucket(K&& k, V&& v)
            : value_(std::forward<K>(k), std::forward<V>(v))
            , status_(BucketStatus::OCCUPIED) {}

        bool is_empty() const noexcept { return status_ == BucketStatus::EMPTY; }
        bool is_occupied() const noexcept { return status_ == BucketStatus::OCCUPIED; }
        bool is_deleted() const noexcept { return status_ == BucketStatus::DELETED; }

        void set_empty() noexcept { status_ = BucketStatus::EMPTY; }
        void set_occupied() noexcept { status_ = BucketStatus::OCCUPIED; }
        void set_deleted() noexcept { status_ = BucketStatus::DELETED; }

        const key_type& key() const { return value_.first; }
        key_type& key() { return value_.first; }

        const mapped_type& value() const { return value_.second; }
        mapped_type& value() { return value_.second; }
    };

    containers::Vector<Bucket> buckets_;
    Hasher hasher_;
    Equal key_equal_;
    size_type size_ = 0;
    size_t seed_;
    mutable size_type count_ = 0;
    float load_factor_ = 0.75f;

    static bool is_prime(size_type n) noexcept {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;
        for (size_type i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0) return false;
        }
        return true;
    }

    static size_type next_prime(size_type n) noexcept {
        if (n <= 2) return 2;
        while (!is_prime(n)) ++n;
        return n;
    }

    size_type hash(const key_type& key, size_type attempt) const noexcept {
        size_t h = hasher_(key) ^ seed_;
        return static_cast<size_type>((h + attempt) % buckets_.size());
    }

    std::optional<size_type> find_pos(const key_type& key) const noexcept {
        if (buckets_.empty()) return std::nullopt;
        size_type attempt = 0;
        size_type pos = hash(key, attempt);

        while (attempt < buckets_.size()) {
            ++count_;
            const auto& bucket = buckets_[pos];
            if (bucket.is_empty()) {
                return std::nullopt;
            }
            if (bucket.is_occupied() && key_equal_(bucket.key(), key)) {
                return pos;
            }
            ++attempt;
            pos = hash(key, attempt);
        }
        return std::nullopt;
    }

    void rehash(size_type new_cap) {
        containers::Vector<Bucket> old_buckets;
        old_buckets.reserve(buckets_.size());
        for (auto& bucket : buckets_) {
            ++count_;
            if (bucket.is_occupied()) {
                old_buckets.push_back(std::move(bucket));
            }
        }
        buckets_.clear();
        buckets_.resize(new_cap);
        size_ = 0;

        for (auto& bucket : old_buckets) {
            ++count_;
            insert_impl(std::move(bucket.key()), std::move(bucket.value()));
        }
    }

    template<typename K, typename V>
    std::pair<iterator, bool> insert_impl(K&& key, V&& value) {
        ++count_;
        if (load_factor() > load_factor_) {
            rehash(next_prime(buckets_.size() * 2));
        }

        size_type attempt = 0;
        size_type pos = hash(key, attempt);
        size_type first_del = buckets_.size();

        while (attempt < buckets_.size()) {
            ++count_;
            const auto& bucket = buckets_[pos];

            if (bucket.is_empty()) {
                size_type insert_pos = (first_del < buckets_.size()) ? first_del : pos;
                buckets_[insert_pos] = Bucket(std::forward<K>(key), std::forward<V>(value));
                ++size_;
                return { iterator(this, insert_pos), true };
            }

            if (bucket.is_deleted() && first_del == buckets_.size()) {
                first_del = pos;
            }

            if (bucket.is_occupied() && key_equal_(bucket.key(), key)) {
                buckets_[pos].value() = std::forward<V>(value);
                return { iterator(this, pos), false };
            }

            ++attempt;
            pos = hash(key, attempt);
        }

        buckets_[first_del] = Bucket(std::forward<K>(key), std::forward<V>(value));
        ++size_;
        return { iterator(this, first_del), true };
    }
};

}