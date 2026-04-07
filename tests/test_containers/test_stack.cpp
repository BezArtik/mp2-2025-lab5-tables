#include "test_common/test_common.hpp"
#include "containers/stack.hpp"
#include <gtest/gtest.h>
#include <utility>
#include <algorithm>

template<typename T>
class StackTest : public tests::TypedContainerTest<T> {};

TYPED_TEST_SUITE(StackTest, tests::FunctionalTypes);

TYPED_TEST(StackTest, can_create_empty_stack) {
	ASSERT_NO_THROW(containers::Stack<TypeParam> stack);
	containers::Stack<TypeParam> stack;
	EXPECT_TRUE(stack.empty());
}

TYPED_TEST(StackTest, can_copy_stack) {
    containers::Stack<TypeParam> stack1;
    TypeParam obj = this->create();
    stack1.push(obj);

    containers::Stack<TypeParam> stack2(stack1);
    ASSERT_EQ(stack2.size(), 1);
    ASSERT_EQ(stack2.top(), obj);
    ASSERT_EQ(stack1.size(), 1);
}

TYPED_TEST(StackTest, can_move_stack) {
    containers::Stack<TypeParam> stack1;
    TypeParam obj = this->create();
    stack1.push(obj);

    containers::Stack<TypeParam> stack2(std::move(stack1));
    ASSERT_EQ(stack2.size(), 1);
    ASSERT_EQ(stack2.top(), obj);
    EXPECT_TRUE(stack1.empty());
}

TYPED_TEST(StackTest, can_push_elenents) {
    containers::Stack<TypeParam> stack;
    TypeParam obj = this->create();

    ASSERT_NO_THROW(stack.push(obj));
    ASSERT_EQ(stack.size(), 1);
    ASSERT_EQ(stack.top(), obj);
}

TYPED_TEST(StackTest, can_push_move) {
    containers::Stack<TypeParam> stack;
    TypeParam obj = this->create();

    ASSERT_NO_THROW(stack.push(std::move(obj)));
    ASSERT_EQ(stack.size(), 1);
}

TYPED_TEST(StackTest, can_pop_elements) {
    containers::Stack<TypeParam> stack;
    TypeParam obj = this->create();
    stack.push(obj);
    ASSERT_NO_THROW(stack.pop());
    EXPECT_TRUE(stack.empty());
}

TYPED_TEST(StackTest, can_clear_stack) {
    auto seq = this->create_sequence(3);
    containers::Stack<TypeParam> stack;
    stack.push(seq[0]);
    stack.push(seq[1]);
    stack.push(seq[2]);
    ASSERT_NO_THROW(stack.clear());
    EXPECT_TRUE(stack.empty());
}