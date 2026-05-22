#pragma once
#include "containers/vector.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include "generators/generators.hpp"
#include <gtest/gtest.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>
#include <algorithm>

namespace tests {

template<typename T>
class ContainerTest : public ::testing::Test {
protected:
    T create() {
        return gen::Generator<T>{}();
    }

    auto create_sequence(size_t n) const {
        containers::Vector<T> result(n);
        std::generate(result.begin(), result.end(), gen::Generator<T>{});
        return result;
    }
};

using FunctionalTypes = ::testing::Types<
    std::string,
    polynomial::Monomial,
    polynomial::Polynomial,
    int, double
>;

}