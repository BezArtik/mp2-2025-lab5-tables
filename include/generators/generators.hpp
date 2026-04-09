#pragma once
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
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

}