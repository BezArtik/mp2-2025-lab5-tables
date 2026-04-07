#pragma once
#include "containers/vector.hpp"

namespace containers {

template <typename T>
class Stack {
public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

	reference top() { return stack_.back(); }
	const_reference top() const { return stack_.back(); }
	bool empty() const noexcept { return stack_.empty(); }
	size_type size() const noexcept { return stack_.size(); }
	void push(const_reference data) { stack_.push_back(data); }
	void push(T&& data) { stack_.push_back(std::move(data)); }
	void pop() noexcept { stack_.pop_back(); }
	void clear() noexcept { stack_.clear(); }
private:
	Vector<T> stack_;
};

}