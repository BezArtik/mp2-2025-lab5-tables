#include "test_common_containers/test_common_containers.hpp"
#include "containers/list.hpp"
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <iterator>

template<typename T>
class ListTest : public tests::TypedContainerTest<T> {};

TYPED_TEST_SUITE(ListTest, tests::FunctionalTypes);

TYPED_TEST(ListTest, can_create_empty_list) {
    ASSERT_NO_THROW(containers::List<TypeParam> list);
    containers::List<TypeParam> list;
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

TYPED_TEST(ListTest, can_create_list_from_initializer_list) {
    auto seq = this->create_sequence(5);
    containers::List<TypeParam> list = { seq[0], seq[1], seq[2], seq[3], seq[4] };
    ASSERT_EQ(list.size(), 5);
    ASSERT_TRUE(std::equal(list.begin(), list.end(), seq.begin()));
}

TYPED_TEST(ListTest, can_copy_list) {
    containers::List<TypeParam> list1;
    auto obj = this->create();
    list1.push_back(obj);

    containers::List<TypeParam> list2(list1);
    ASSERT_EQ(list2.size(), 1);
    ASSERT_EQ(list2.front(), obj);
    ASSERT_EQ(list1.size(), 1);
}

TYPED_TEST(ListTest, can_move_list) {
    containers::List<TypeParam> list1;
    auto obj = this->create();
    list1.push_back(obj);

    containers::List<TypeParam> list2(std::move(list1));
    ASSERT_EQ(list2.size(), 1);
    ASSERT_EQ(list2.front(), obj);
    ASSERT_EQ(list1.size(), 0);
}

TYPED_TEST(ListTest, can_push_back_elements) {
    containers::List<TypeParam> list;
    auto obj = this->create();

    ASSERT_NO_THROW(list.push_back(obj));
    ASSERT_EQ(list.size(), 1);
    ASSERT_EQ(list.front(), obj);

    auto obj2 = this->create();
    list.push_back(obj2);
    ASSERT_EQ(list.size(), 2);
    ASSERT_EQ(list.back(), obj2);
}

TYPED_TEST(ListTest, can_push_back_move) {
    containers::List<TypeParam> list;
    auto obj = this->create();

    list.push_back(std::move(obj));
    ASSERT_EQ(list.size(), 1);
}

TYPED_TEST(ListTest, can_emplace_back_elements) {
    containers::List<TypeParam> list;
    auto obj = this->create();

    ASSERT_NO_THROW(list.emplace_back(obj));
    ASSERT_EQ(list.size(), 1);
    ASSERT_EQ(list.front(), obj);
}

TYPED_TEST(ListTest, can_insert_at_beginning) {
    containers::List<TypeParam> list;
    auto obj1 = this->create();
    auto obj2 = this->create();

    list.push_back(obj1);
    auto it = list.insert(list.begin(), obj2);

    ASSERT_EQ(list.size(), 2);
    ASSERT_EQ(list.front(), obj2);
    ASSERT_EQ(list.back(), obj1);
    ASSERT_EQ(it, list.begin());
}

TYPED_TEST(ListTest, can_erase_elements) {
    containers::List<TypeParam> list;
    list.push_back(this->create());

    auto it = list.erase(list.begin());
    ASSERT_EQ(list.size(), 0);
    ASSERT_EQ(it, list.end());
}

TYPED_TEST(ListTest, can_pop_back) {
    containers::List<TypeParam> list;
    list.push_back(this->create());

    ASSERT_NO_THROW(list.pop_back());
    ASSERT_EQ(list.size(), 0);
}

TYPED_TEST(ListTest, can_clear_list) {
    containers::List<TypeParam> list;
    list.push_back(this->create());
    list.push_back(this->create());

    ASSERT_NO_THROW(list.clear());
    ASSERT_TRUE(list.empty());
}

TYPED_TEST(ListTest, can_access_front_and_back) {
    auto seq = this->create_sequence(2);
    containers::List<TypeParam> list;
    std::copy(seq.begin(), seq.end(), std::back_inserter(list));

    ASSERT_EQ(list.front(), seq[0]);
    ASSERT_EQ(list.back(), seq[1]);
}

TYPED_TEST(ListTest, iterators_work) {
    auto seq = this->create_sequence(2);
    containers::List<TypeParam> list;
    std::copy(seq.begin(), seq.end(), std::back_inserter(list));

    auto it = list.begin();
    ASSERT_EQ(*it, seq[0]);
    ++it;
    ASSERT_EQ(*it, seq[1]);
    ++it;
    ASSERT_EQ(it, list.end());
}

TYPED_TEST(ListTest, const_iterators_work) {
    containers::List<TypeParam> list;
    list.push_back(this->create());

    const auto& const_list = list;
    auto it = const_list.begin();
    ASSERT_EQ(*it, const_list.front());
    ++it;
    ASSERT_EQ(it, const_list.end());
}

TYPED_TEST(ListTest, can_swap_lists) {
    containers::List<TypeParam> list1;
    auto obj1 = this->create();
    list1.push_back(obj1);

    containers::List<TypeParam> list2;
    auto obj2 = this->create();
    list2.push_back(obj2);

    std::swap(list1, list2);

    ASSERT_EQ(list1.size(), 1);
    ASSERT_EQ(list1.front(), obj2);


    ASSERT_EQ(list2.size(), 1);
    ASSERT_EQ(list2.front(), obj1);

}

TYPED_TEST(ListTest, sort_works_on_empty_list) {
    containers::List<TypeParam> list;
    ASSERT_NO_THROW(list.sort());
    ASSERT_TRUE(list.empty());
}

TYPED_TEST(ListTest, sort_works_on_single_element) {
    containers::List<TypeParam> list;
    auto obj = this->create();
    list.push_back(obj);

    list.sort();
    ASSERT_EQ(list.size(), 1);
    ASSERT_EQ(list.front(), obj);
}

TYPED_TEST(ListTest, sort_works_on_sorted_list) {
    auto seq = this->create_sequence(50);
    containers::List<TypeParam> list;
    std::copy(seq.begin(), seq.end(), std::back_inserter(list));

    list.sort();

    ASSERT_TRUE(std::is_sorted(list.begin(), list.end()));
    ASSERT_EQ(list.size(), 50);
}

TYPED_TEST(ListTest, merge_works_on_empty_lists) {
    containers::List<TypeParam> list1;
    containers::List<TypeParam> list2;

    list1.merge(std::move(list2));
    ASSERT_TRUE(list1.empty());
    ASSERT_TRUE(list2.empty());
}

TYPED_TEST(ListTest, merge_works_when_first_is_empty) {
    containers::List<TypeParam> list1;
    containers::List<TypeParam> list2;

    auto seq = this->create_sequence(10);
    std::copy(seq.begin(), seq.end(), std::back_inserter(list2));
    list2.sort();

    list1.merge(std::move(list2));

    ASSERT_EQ(list1.size(), 10);
    ASSERT_TRUE(list2.empty());
    ASSERT_TRUE(std::is_sorted(list1.begin(), list1.end()));
}

TYPED_TEST(ListTest, merge_works_when_second_is_empty) {
    containers::List<TypeParam> list1;
    containers::List<TypeParam> list2;

    auto seq = this->create_sequence(10);
    std::copy(seq.begin(), seq.end(), std::back_inserter(list1));
    list1.sort();

    list1.merge(std::move(list2));

    ASSERT_EQ(list1.size(), 10);
    ASSERT_TRUE(list2.empty());
    ASSERT_TRUE(std::is_sorted(list1.begin(), list1.end()));
}

TYPED_TEST(ListTest, merge_combines_two_sorted_lists) {
    containers::List<TypeParam> list1;
    containers::List<TypeParam> list2;

    auto seq1 = this->create_sequence(50);
    auto seq2 = this->create_sequence(50);

    std::copy(seq1.begin(), seq1.end(), std::back_inserter(list1));
    std::copy(seq2.begin(), seq2.end(), std::back_inserter(list2));

    list1.sort();
    list2.sort();

    list1.merge(std::move(list2));

    EXPECT_EQ(list1.size(), 100);
    EXPECT_TRUE(list2.empty());
    EXPECT_TRUE(std::is_sorted(list1.begin(), list1.end()));
}

TYPED_TEST(ListTest, merge_preserves_total_elements_count) {
    containers::List<TypeParam> list1;
    containers::List<TypeParam> list2;

    size_t n1 = 73;
    size_t n2 = 84;

    auto seq1 = this->create_sequence(n1);
    auto seq2 = this->create_sequence(n2);

    std::copy(seq1.begin(), seq1.end(), std::back_inserter(list1));
    std::copy(seq2.begin(), seq2.end(), std::back_inserter(list2));

    list1.sort();
    list2.sort();

    list1.merge(std::move(list2));

    EXPECT_EQ(list1.size(), n1 + n2);
    EXPECT_TRUE(list2.empty());
    EXPECT_TRUE(std::is_sorted(list1.begin(), list1.end()));
}