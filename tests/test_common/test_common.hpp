#pragma once
#include "containers/vector.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include <gtest/gtest.h>
#include <gtest/internal/gtest-type-util.h>
#include <random>
#include <string>
#include <type_traits>
#include <algorithm>

namespace tests {

struct Tracker {
    static size_t copy_count_;
    static size_t move_count_;
    static size_t default_count_;
    static size_t destruct_count_;

    size_t value_;

    Tracker();
    Tracker(size_t v);
    Tracker(const Tracker& other);
    Tracker(Tracker&& other) noexcept;
    ~Tracker();

    Tracker& operator=(const Tracker& other);
    Tracker& operator=(Tracker&& other) noexcept;

    bool operator==(const Tracker& other) const noexcept;
    bool operator<(const Tracker& other) const noexcept;

    static void reset_counts();
};

class RandomStringGenerator {
public:
    explicit RandomStringGenerator(size_t min_len = 10, size_t max_len = 500);
    std::string operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<> len_dis_;
    std::uniform_int_distribution<> char_dis_;
    static const char* chars_;
};

class RandomMonomialGenerator {
public:
    RandomMonomialGenerator(double coeff_min = -100.0, double coeff_max = 100.0);
    polynomial::Monomial operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_real_distribution<> coeff_dis_;
};

class RandomPolynomialGenerator {
public:
    explicit RandomPolynomialGenerator(size_t max_monomials = 250);
    polynomial::Polynomial operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<> monom_count_dis_;
    RandomMonomialGenerator monom_gen_;
};


template<typename T>
class TypedContainerTest : public ::testing::Test {
protected:
    T create() {
        if constexpr (std::is_same_v<T, std::string>) {
            return RandomStringGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Monomial>) {
            return RandomMonomialGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Polynomial>) {
            return RandomPolynomialGenerator{}();
        } else {
            return T{};
        }
    }

    containers::Vector<T> create_sequence(size_t n) {
        containers::Vector<T> result(n);
        if constexpr (std::is_same_v<T, std::string>) {
            std::generate_n(result.begin(), n, RandomStringGenerator{});
        } else if constexpr (std::is_same_v<T, polynomial::Monomial>) {
            std::generate_n(result.begin(), n, RandomMonomialGenerator{});
        } else if constexpr (std::is_same_v<T, polynomial::Polynomial>) {
            std::generate_n(result.begin(), n, RandomPolynomialGenerator{});
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