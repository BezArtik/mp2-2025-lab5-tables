#include "test_common/test_common.hpp"
#include "containers/sorted_list.hpp"
#include <algorithm>

using tests::TypedContainerTest;
using tests::FunctionalTypes;

template<typename T>
class SortedListTest : public TypedContainerTest<T> {};

TYPED_TEST_SUITE(SortedListTest, FunctionalTypes);

TYPED_TEST(SortedListTest, can_create_empty) {
    containers::SortedList<TypeParam> list;
    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
}

TYPED_TEST(SortedListTest, can_create_from_initializer_list) {
    auto seq = this->create_sequence(5);
    containers::SortedList<TypeParam> list = { seq[0], seq[1], seq[2], seq[3], seq[4] };

    ASSERT_EQ(list.size(), 5);
    ASSERT_TRUE(std::is_sorted(list.begin(), list.end()));
}

TYPED_TEST(SortedListTest, can_create_from_iterator_range) {
    auto seq = this->create_sequence(10);
    containers::SortedList<TypeParam> list(seq.begin(), seq.end());

    ASSERT_EQ(list.size(), 10);
    ASSERT_TRUE(std::is_sorted(list.begin(), list.end()));
}


TYPED_TEST(SortedListTest, insert_maintains_sorted_order) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    ASSERT_EQ(list.size(), 10);
    ASSERT_TRUE(std::is_sorted(list.begin(), list.end()));
}

TYPED_TEST(SortedListTest, insert_unique_prevents_duplicates) {
    containers::SortedList<TypeParam> list;
    auto obj = this->create();

    auto it1 = list.insert_unique(obj);
    auto it2 = list.insert_unique(obj);

    ASSERT_EQ(list.size(), 1);
    ASSERT_EQ(it1, it2);
    ASSERT_TRUE(std::is_sorted(list.begin(), list.end()));
}

TYPED_TEST(SortedListTest, insert_unique_with_move) {
    containers::SortedList<TypeParam> list;
    auto obj = this->create();

    list.insert_unique(obj);
    list.insert_unique(std::move(obj)); 

    ASSERT_EQ(list.size(), 1);
}


TYPED_TEST(SortedListTest, find_existing_element) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    for (const auto& val : seq) {
        auto it = list.find(val);
        ASSERT_NE(it, list.end());
        ASSERT_EQ(*it, val);
    }
}

TYPED_TEST(SortedListTest, find_non_existing_element) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    auto not_in_list = this->create();  
    auto it = list.find(not_in_list);

    if (it != list.end()) {
        ASSERT_EQ(*it, not_in_list);
    }
}

TYPED_TEST(SortedListTest, const_find_works) {
    containers::SortedList<TypeParam> list;
    auto obj = this->create();
    list.insert(obj);

    const auto& const_list = list;
    auto it = const_list.find(obj);

    ASSERT_NE(it, const_list.end());
    ASSERT_EQ(*it, obj);
}


TYPED_TEST(SortedListTest, erase_element) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    auto it = list.find(seq[0]);
    ASSERT_NE(it, list.end());

    auto it1 = list.erase(it);
    ASSERT_TRUE(std::is_sorted(list.begin(), list.end()));

    it = list.find(seq[0]);
    ASSERT_EQ(it, list.end());

}


TYPED_TEST(SortedListTest, merge_two_sorted_lists) {
    auto seq1 = this->create_sequence(20);
    auto seq2 = this->create_sequence(20);

    containers::SortedList<TypeParam> list1(seq1.begin(), seq1.end());
    containers::SortedList<TypeParam> list2(seq2.begin(), seq2.end());

    list1.merge(std::move(list2));

    ASSERT_EQ(list1.size(), 40);
    ASSERT_TRUE(list2.empty());
    ASSERT_TRUE(std::is_sorted(list1.begin(), list1.end()));
}

TYPED_TEST(SortedListTest, merge_with_empty_list) {
    auto seq = this->create_sequence(10);
    containers::SortedList<TypeParam> list1(seq.begin(), seq.end());
    containers::SortedList<TypeParam> list2;

    list1.merge(std::move(list2));

    ASSERT_EQ(list1.size(), 10);
    ASSERT_TRUE(list2.empty());
    ASSERT_TRUE(std::is_sorted(list1.begin(), list1.end()));
}

TYPED_TEST(SortedListTest, merge_empty_into_empty) {
    containers::SortedList<TypeParam> list1;
    containers::SortedList<TypeParam> list2;

    list1.merge(std::move(list2));

    ASSERT_TRUE(list1.empty());
    ASSERT_TRUE(list2.empty());
}


TYPED_TEST(SortedListTest, front_returns_smallest_element) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    auto min_it = std::min_element(seq.begin(), seq.end());
    ASSERT_EQ(list.front(), *min_it);
}

TYPED_TEST(SortedListTest, back_returns_largest_element) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    auto max_it = std::max_element(seq.begin(), seq.end());
    ASSERT_EQ(list.back(), *max_it);
}


TYPED_TEST(SortedListTest, clear_removes_all_elements) {
    containers::SortedList<TypeParam> list;
    auto seq = this->create_sequence(10);

    for (const auto& val : seq) {
        list.insert(val);
    }

    ASSERT_FALSE(list.empty());
    list.clear();
    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0);
}


TYPED_TEST(SortedListTest, swap_exchanges_contents) {
    auto seq1 = this->create_sequence(5);
    auto seq2 = this->create_sequence(10);

    containers::SortedList<TypeParam> list1(seq1.begin(), seq1.end());
    containers::SortedList<TypeParam> list2(seq2.begin(), seq2.end());

    auto size1 = list1.size();
    auto size2 = list2.size();

    list1.swap(list2);

    ASSERT_EQ(list1.size(), size2);
    ASSERT_EQ(list2.size(), size1);
    ASSERT_TRUE(std::is_sorted(list1.begin(), list1.end()));
    ASSERT_TRUE(std::is_sorted(list2.begin(), list2.end()));
}

