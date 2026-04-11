#pragma once
#include "containers/vector.hpp"
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "generators/generators.hpp"
#include <gtest/gtest.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>
#include <type_traits>
#include <algorithm>

namespace tests {

template<typename T>
class TypedContainerTest : public ::testing::Test {
protected:
    T create() {
        if constexpr (std::is_same_v<T, std::string>) {
            return gen::StringGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Monomial>) {
            return gen::MonomialGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Polynomial>) {
            return gen::PolynomialGenerator{}();
        } else {
            return T{};
        }
    }

    auto create_sequence(size_t n) {
        containers::Vector<T> result(n);
        if constexpr (std::is_same_v<T, std::string>) {
            std::generate_n(result.begin(), n, gen::StringGenerator{});
        } else if constexpr (std::is_same_v<T, polynomial::Monomial>) {
            std::generate_n(result.begin(), n, gen::MonomialGenerator{});
        } else if constexpr (std::is_same_v<T, polynomial::Polynomial>) {
            std::generate_n(result.begin(), n, gen::PolynomialGenerator{});
        }
        return result;
    }
};

using FunctionalTypes = ::testing::Types<
    std::string,
    polynomial::Monomial,
    polynomial::Polynomial
>;

}