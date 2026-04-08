#pragma once
#include "containers/vector.hpp"
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "generators/generators.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

namespace tests {

template<typename T>
struct Generator {
    T operator()() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return gen::StringGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Monomial>) {
            return gen::MonomialGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Polynomial>) {
            return gen::PolynomialGenerator{}();
        } else if constexpr (std::is_same_v<T, int>) {
            static std::mt19937_64 gen(std::random_device{}());
            static std::uniform_int_distribution<int> dist(1, 1000);
            return dist(gen);
        } else if constexpr (std::is_same_v<T, double>) {
            static std::mt19937_64 gen(std::random_device{}());
            static std::uniform_real_distribution<double> dist(0.0, 100.0);
            return dist(gen);
        }
        else {
            return T{};
        }
    }
};

template<typename Key, typename Value>
struct PairGenerator {
    std::pair<Key, Value> operator()() const {
        return { Generator<Key>{}(), Generator<Value>{}() };
    }
};

template<typename TableType>
class TableTest : public ::testing::Test {
protected:
    using Table = TableType;
    using Key = typename Table::key_type;
    using Value = typename Table::mapped_type;
    using Pair = typename Table::value_type;

    Table table;

    Key create_key() const { return Generator<Key>{}(); }
    Value create_value() const { return Generator<Value>{}(); }
    Pair create_pair() const { return PairGenerator<Key, Value>{}(); }

    containers::Vector<Pair> create_pair_sequence(size_t n) const {
        containers::Vector<Pair> result(n);
        std::generate_n(result.begin(), n, PairGenerator<Key, Value>{});
        return result;
    }

    template <typename InputIt>
    Key create_unique_key(InputIt first, InputIt last) const {
        const int max_attempts = 1000;
        for (int attempt = 0; attempt < max_attempts; ++attempt) {
            Key new_key = create_key();
            bool exists = std::any_of(first, last,
                [&new_key](const Pair& p) {
                    return p.first == new_key;
                });
            if (!exists) return new_key;
        }
        throw std::runtime_error("Failed to generate unique key");
    }

    template<typename InputIt>
    void insert_sequence(InputIt first, InputIt last) {
        std::for_each(first, last, [this](const auto& p) { table.insert(p); });
    }

    template<typename InputIt>
    bool all_keys_present(InputIt first, InputIt last) const {
        return std::all_of(first, last,
            [this](const auto& p) {
                return table.find(p.first) != table.end();
            });
    }

    template<typename InputIt>
    size_t count_successful_finds(InputIt first, InputIt last) const {
        return std::count_if(first, last,
            [this](const auto& p) {
                return table.find(p.first) != table.end();
            });
    }
};

TYPED_TEST_SUITE_P(TableTest);

TYPED_TEST_P(TableTest, insert_new_elements) {
    auto pairs = this->create_pair_sequence(10);

    containers::Vector<bool> insert_results;
    std::transform(pairs.begin(), pairs.end(), std::back_inserter(insert_results),
        [this](const auto& p) { return this->table.insert(p).second; });

    EXPECT_TRUE(std::all_of(insert_results.begin(), insert_results.end(),
        [](bool b) { return b; }));
    EXPECT_EQ(this->table.size(), pairs.size());
    EXPECT_TRUE(this->all_keys_present(pairs.begin(), pairs.end()));
}

TYPED_TEST_P(TableTest, insert_existing_key) {
    auto pair = this->create_pair();
    auto new_value = this->create_value();
    auto updated_pair = std::make_pair(pair.first, new_value);

    auto [it1, inserted1] = this->table.insert(pair);
    auto [it2, inserted2] = this->table.insert(updated_pair);

    EXPECT_TRUE(inserted1);
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(it1, it2);
    EXPECT_EQ(it1->second, new_value);
    EXPECT_EQ(this->table.size(), 1);
}

TYPED_TEST_P(TableTest, find_operations) {
    auto pairs = this->create_pair_sequence(20);
    this->insert_sequence(pairs.begin(), pairs.end());

    EXPECT_EQ(this->count_successful_finds(pairs.begin(), pairs.end()), pairs.size());

    auto non_existent = this->create_unique_key(pairs.begin(), pairs.end());

    EXPECT_EQ(this->table.find(non_existent), this->table.end());
}

TYPED_TEST_P(TableTest, erase_by_key) {
    auto pairs = this->create_pair_sequence(15);
    this->insert_sequence(pairs.begin(), pairs.end());

    auto original_size = this->table.size();

    for (size_t i = 0; i < pairs.size(); i += 3) {
        this->table.erase(pairs[i].first);
    }

    EXPECT_EQ(this->table.size(), original_size - (pairs.size() + 2) / 3);

    for (size_t i = 0; i < pairs.size(); i += 3) {
        EXPECT_EQ(this->table.find(pairs[i].first), this->table.end());
    }

    for (size_t i = 0; i < pairs.size(); ++i) {
        if (i % 3 != 0) {
            EXPECT_NE(this->table.find(pairs[i].first), this->table.end());
        }
    }
}

TYPED_TEST_P(TableTest, erase_nonexistent_key) {
    auto pairs = this->create_pair_sequence(10); 
    this->insert_sequence(pairs.begin(), pairs.end());

    auto non_existent = this->create_unique_key(pairs.begin(), pairs.end());

    size_t size_before = this->table.size();
    auto result = this->table.erase(non_existent);

    EXPECT_EQ(result, this->table.end());
    EXPECT_EQ(this->table.size(), size_before);

    EXPECT_TRUE(this->all_keys_present(pairs.begin(), pairs.end()));
}

TYPED_TEST_P(TableTest, erase_by_iterator) {
    auto pairs = this->create_pair_sequence(5); 
    this->insert_sequence(pairs.begin(), pairs.end());

    auto it = this->table.begin();
    std::advance(it, 2);
    auto next = this->table.erase(it);

    EXPECT_EQ(this->table.size(), 4);
    EXPECT_NE(next, this->table.end());

    while (!this->table.empty()) {
        this->table.erase(this->table.begin());
    }

    EXPECT_TRUE(this->table.empty());
}

TYPED_TEST_P(TableTest, clear_operations) {
    auto pairs = this->create_pair_sequence(10);
    this->insert_sequence(pairs.begin(), pairs.end());

    EXPECT_FALSE(this->table.empty());
    this->table.clear();
    EXPECT_TRUE(this->table.empty());
    EXPECT_EQ(this->table.size(), 0);
    EXPECT_EQ(this->table.begin(), this->table.end());
}

TYPED_TEST_P(TableTest, iterator_traversal) {
    auto pairs = this->create_pair_sequence(8); 
    this->insert_sequence(pairs.begin(), pairs.end());

    size_t count = std::distance(this->table.begin(), this->table.end());
    EXPECT_EQ(count, pairs.size());

    count = 0;
    for ([[maybe_unused]] const auto& _ : this->table) {
        ++count;
    }
    EXPECT_EQ(count, pairs.size());

    const auto& const_table = this->table;
    count = std::distance(const_table.begin(), const_table.end());
    EXPECT_EQ(count, pairs.size());
}

TYPED_TEST_P(TableTest, move_semantics) {
    auto key = this->create_key();
    auto value1 = this->create_value();
    auto value2 = this->create_value();

    auto [it1, inserted1] = this->table.insert({ key, std::move(value1) });
    EXPECT_TRUE(inserted1);

    auto [it2, inserted2] = this->table.insert({ key, std::move(value2) });
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(it1, it2);
}

REGISTER_TYPED_TEST_SUITE_P(TableTest,
    insert_new_elements,
    insert_existing_key,
    find_operations,
    erase_by_key,
    erase_nonexistent_key,
    erase_by_iterator,
    clear_operations,
    iterator_traversal,
    move_semantics
);

} 