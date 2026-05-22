#include "test_common/test_common_containers.hpp"
#include "containers/vector.hpp"
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <iterator>

namespace tests {

template<typename T>
class VectorTest : public ContainerTest<T> {};

TYPED_TEST_SUITE(VectorTest, FunctionalTypes);

TYPED_TEST(VectorTest, can_create_empty) {
    ASSERT_NO_THROW(containers::Vector<TypeParam> vec);
    containers::Vector<TypeParam> vec;
    ASSERT_EQ(vec.size(), 0);
    ASSERT_TRUE(vec.empty());
    ASSERT_EQ(vec.capacity(), 0);
}

TYPED_TEST(VectorTest, can_create_with_size) {
    containers::Vector<TypeParam> vec(5);
    ASSERT_EQ(vec.size(), 5);
    ASSERT_GE(vec.capacity(), 5);
}

TYPED_TEST(VectorTest, can_create_with_size_and_value) {
    auto obj = this->create();
    containers::Vector<TypeParam> vec(5, obj);
    ASSERT_EQ(vec.size(), 5);
    ASSERT_TRUE(std::all_of(vec.begin(), vec.end(),
        [&obj](const auto& elem) {
            return elem == obj;
        }));
}

TYPED_TEST(VectorTest, can_create_from_initializer_list) {
    auto seq = this->create_sequence(5);
    containers::Vector<TypeParam> vec = { seq[0], seq[1], seq[2], seq[3], seq[4] };
    ASSERT_EQ(vec.size(), 5);
    ASSERT_TRUE(std::equal(vec.begin(), vec.end(), seq.begin()));
}

TYPED_TEST(VectorTest, can_copy) {
    containers::Vector<TypeParam> vec1;
    auto obj = this->create();
    vec1.push_back(obj);

    containers::Vector<TypeParam> vec2(vec1);
    ASSERT_EQ(vec2.size(), 1);
    ASSERT_EQ(vec2[0], obj);
    ASSERT_EQ(vec1.size(), 1);
}

TYPED_TEST(VectorTest, can_move) {
    containers::Vector<TypeParam> vec1;
    auto obj = this->create();
    vec1.push_back(obj);
    auto* old_data = vec1.data();

    containers::Vector<TypeParam> vec2(std::move(vec1));
    ASSERT_EQ(vec2.size(), 1);
    ASSERT_EQ(vec2[0], obj);
    ASSERT_EQ(vec1.size(), 0);
    ASSERT_EQ(vec1.data(), nullptr);
    ASSERT_EQ(vec2.data(), old_data);
}

TEST(VectorExceptionTest, out_of_range_on_at) {
    containers::Vector<size_t> vec;
    ASSERT_THROW(vec.at(0), std::out_of_range);

    vec.push_back(1);
    ASSERT_THROW(vec.at(1), std::out_of_range);
    ASSERT_NO_THROW(vec.at(0));
}

TYPED_TEST(VectorTest, can_push_back_elements) {
    containers::Vector<TypeParam> vec;
    auto obj = this->create();

    ASSERT_NO_THROW(vec.push_back(obj));
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec[0], obj);

    auto obj2 = this->create();
    vec.push_back(obj2);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[1], obj2);
}

TYPED_TEST(VectorTest, can_push_back_move) {
    containers::Vector<TypeParam> vec;
    auto obj = this->create();

    ASSERT_NO_THROW(vec.push_back(std::move(obj)));
    ASSERT_EQ(vec.size(), 1);
}

TYPED_TEST(VectorTest, can_emplace_back_elements) {
    containers::Vector<TypeParam> vec;
    auto obj = this->create();

    ASSERT_NO_THROW(vec.emplace_back(obj));
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec[0], obj);
}

TYPED_TEST(VectorTest, can_insert_at_beginning) {
    containers::Vector<TypeParam> vec;
    auto obj1 = this->create();
    auto obj2 = this->create();

    vec.push_back(obj1);
    auto it = vec.insert(vec.begin(), obj2);

    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[0], obj2);
    ASSERT_EQ(vec[1], obj1);
    ASSERT_EQ(it, vec.begin());
}

TYPED_TEST(VectorTest, can_insert_in_middle) {
    auto seq = this->create_sequence(5);
    containers::Vector<TypeParam> vec;
    std::copy_n(seq.begin(), 2, std::back_inserter(vec));
    std::copy_n(seq.begin() + 3, 2, std::back_inserter(vec));

    auto it = vec.insert(vec.begin() + 2, seq[2]);

    ASSERT_EQ(vec.size(), 5);
    ASSERT_TRUE(std::equal(vec.begin(), vec.end(), seq.begin()));
    ASSERT_EQ(it, vec.begin() + 2);
}

TYPED_TEST(VectorTest, can_erase_elements) {
    containers::Vector<TypeParam> vec;
    auto obj = this->create();
    vec.push_back(obj);

    auto it = vec.erase(vec.begin());
    ASSERT_EQ(vec.size(), 0);
    ASSERT_EQ(it, vec.end());
}

TYPED_TEST(VectorTest, can_erase_in_middle) {
    auto seq = this->create_sequence(5);
    containers::Vector<TypeParam> vec;
    std::copy(seq.begin(), seq.end(), std::back_inserter(vec));

    auto it = vec.erase(vec.begin() + 2);

    ASSERT_EQ(vec.size(), 4);
    ASSERT_EQ(vec[0], seq[0]);
    ASSERT_EQ(vec[1], seq[1]);
    ASSERT_EQ(vec[2], seq[3]);
    ASSERT_EQ(vec[3], seq[4]);
    ASSERT_EQ(it, vec.begin() + 2);
}

TYPED_TEST(VectorTest, can_pop_back) {
    containers::Vector<TypeParam> vec;
    auto obj = this->create();
    vec.push_back(obj);

    ASSERT_NO_THROW(vec.pop_back());
    ASSERT_EQ(vec.size(), 0);
}

TYPED_TEST(VectorTest, can_clear) {
    containers::Vector<TypeParam> vec;
    vec.push_back(this->create());
    vec.push_back(this->create());

    ASSERT_NO_THROW(vec.clear());
    ASSERT_TRUE(vec.empty());
}

TYPED_TEST(VectorTest, can_reserve_capacity) {
    containers::Vector<TypeParam> vec;
    ASSERT_NO_THROW(vec.reserve(10));
    ASSERT_GE(vec.capacity(), 10);
    ASSERT_EQ(vec.size(), 0);

    vec.reserve(5);
    ASSERT_GE(vec.capacity(), 10);
}

TYPED_TEST(VectorTest, can_shrink_to_fit) {
    containers::Vector<TypeParam> vec;
    vec.push_back(this->create());
    vec.reserve(100);

    auto old_cap = vec.capacity();
    ASSERT_GE(old_cap, 100);

    vec.shrink_to_fit();
    ASSERT_LE(vec.capacity(), old_cap);
    ASSERT_GE(vec.capacity(), vec.size());
    ASSERT_EQ(vec.size(), 1);
}

TYPED_TEST(VectorTest, resize_increases_size) {
    containers::Vector<TypeParam> vec;
    auto obj = this->create();

    vec.resize(5, obj);
    ASSERT_EQ(vec.size(), 5);
    ASSERT_TRUE(std::all_of(vec.begin(), vec.end(),
        [&obj](const auto& elem) {
            return elem == obj;
        }));
}

TYPED_TEST(VectorTest, resize_decreases_size) {
    containers::Vector<TypeParam> vec;
    auto seq = this->create_sequence(10);
    std::copy(seq.begin(), seq.end(), std::back_inserter(vec));

    vec.resize(3);
    ASSERT_EQ(vec.size(), 3);
    ASSERT_TRUE(std::equal(vec.begin(), vec.end(), seq.begin()));
}

TYPED_TEST(VectorTest, can_access_front_and_back) {
    auto seq = this->create_sequence(2);
    containers::Vector<TypeParam> vec;
    std::copy(seq.begin(), seq.end(), std::back_inserter(vec));

    ASSERT_EQ(vec.front(), seq[0]);
    ASSERT_EQ(vec.back(), seq[1]);
}

TYPED_TEST(VectorTest, can_get_data_pointer) {
    containers::Vector<TypeParam> vec;
    vec.push_back(this->create());

    auto* data = vec.data();
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(*data, vec[0]);

    const auto& const_vec = vec;
    const auto* const_data = const_vec.data();
    ASSERT_EQ(const_data, data);
}

TYPED_TEST(VectorTest, iterators_work) {
    auto seq = this->create_sequence(2);
    containers::Vector<TypeParam> vec;
    std::copy(seq.begin(), seq.end(), std::back_inserter(vec));

    auto it = vec.begin();
    ASSERT_EQ(*it, seq[0]);
    ++it;
    ASSERT_EQ(*it, seq[1]);
    ++it;
    ASSERT_EQ(it, vec.end());
}

TYPED_TEST(VectorTest, const_iterators_work) {
    containers::Vector<TypeParam> vec;
    vec.push_back(this->create());

    const auto& const_vec = vec;
    auto it = const_vec.begin();
    ASSERT_EQ(*it, vec[0]);
    ++it;
    ASSERT_EQ(it, const_vec.end());
}

TYPED_TEST(VectorTest, iterator_random_access) {
    auto seq = this->create_sequence(5);
    containers::Vector<TypeParam> vec;
    std::copy(seq.begin(), seq.end(), std::back_inserter(vec));

    auto it = vec.begin();
    it += 2;
    ASSERT_EQ(*it, seq[2]);

    auto it2 = it - 1;
    ASSERT_EQ(*it2, seq[1]);

    auto diff = it - vec.begin();
    ASSERT_EQ(diff, 2);

    ASSERT_LT(vec.begin(), it);
    ASSERT_GT(vec.end(), it);

    ASSERT_EQ(it[0], seq[2]);
    ASSERT_EQ(it[1], seq[3]);

    it -= 1;
    ASSERT_EQ(*it, seq[1]);
    it += 2;
    ASSERT_EQ(*it, seq[3]);
}

TYPED_TEST(VectorTest, can_swaps) {
    containers::Vector<TypeParam> vec1;
    auto obj1 = this->create();
    vec1.push_back(obj1);

    containers::Vector<TypeParam> vec2;
    auto obj2 = this->create();
    vec2.push_back(obj2);

    auto* data1 = vec1.data();
    auto* data2 = vec2.data();

    std::swap(vec1, vec2);

    ASSERT_EQ(vec1.size(), 1);
    ASSERT_EQ(vec1[0], obj2);
    ASSERT_EQ(vec1.data(), data2);

    ASSERT_EQ(vec2.size(), 1);
    ASSERT_EQ(vec2[0], obj1);
    ASSERT_EQ(vec2.data(), data1);
}

TYPED_TEST(VectorTest, can_store_unique_ptr) {
    containers::Vector<std::unique_ptr<TypeParam>> vec;
    auto obj1 = this->create();
    auto obj2 = this->create();
    vec.push_back(std::make_unique<TypeParam>(obj1));
    vec.push_back(std::make_unique<TypeParam>(obj2));

    ASSERT_EQ(*vec[0], obj1);
    ASSERT_EQ(*vec[1], obj2);

    containers::Vector<std::unique_ptr<TypeParam>> vec2(std::move(vec));
    ASSERT_EQ(vec2.size(), 2);
    ASSERT_EQ(*vec2[0], obj1);
    ASSERT_EQ(vec.size(), 0);
}

TYPED_TEST(VectorTest, can_insert_move_only) {
    containers::Vector<std::unique_ptr<TypeParam>> vec;
    auto obj = this->create();
    auto ptr = std::make_unique<TypeParam>(obj);

    vec.insert(vec.begin(), std::move(ptr));
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(*vec[0], obj);
    ASSERT_EQ(ptr, nullptr);
}

TYPED_TEST(VectorTest, can_emplace_back_move_only) {
    containers::Vector<std::unique_ptr<TypeParam>> vec;
    auto obj = this->create();
    vec.emplace_back(std::make_unique<TypeParam>(obj));
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(*vec[0], obj);
}

}