#pragma once
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include <random>

namespace gen {

class StringGenerator {
public:
    StringGenerator(size_t min_len = 50, size_t max_len = 500);
    std::string operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<size_t> len_dis_;
    std::uniform_int_distribution<size_t> char_dis_;
    const char* chars_ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
};

class MonomialGenerator {
public:
    MonomialGenerator(double coeff_min = -100.0, double coeff_max = 100.0);
    polynomial::Monomial operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_real_distribution<> coeff_dis_;
};

class PolynomialGenerator {
public:
    PolynomialGenerator(size_t max_monomials = 300);
    polynomial::Polynomial operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<size_t> monom_count_dis_;
    MonomialGenerator monom_gen_;
};

template <typename T>
struct Generator {
    T operator()() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return StringGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Monomial>) {
            return MonomialGenerator{}();
        } else if constexpr (std::is_same_v<T, polynomial::Polynomial>) {
            return PolynomialGenerator{}();
        } else if constexpr (std::is_same_v<T, int>) {
            static std::mt19937_64 gen(std::random_device{}());
            static std::uniform_int_distribution dist(-10'000, 10'000);
            return dist(gen);
        } else if constexpr (std::is_same_v<T, double>) {
            static std::mt19937_64 gen(std::random_device{}());
            static std::uniform_real_distribution dist(-10'000.0, 10'000.0);
            return dist(gen);
        } else {
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

}