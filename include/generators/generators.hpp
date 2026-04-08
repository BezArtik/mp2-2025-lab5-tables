#pragma once
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include <random>
#include <string>
#include <algorithm>
#include <cstdint>
#include <iterator>

namespace gen {

class StringGenerator {
public:
    StringGenerator(size_t min_len = 50, size_t max_len = 500)
        : gen_(std::random_device{}())
        , len_dis_(min_len, max_len)
        , char_dis_(0, sizeof(chars_) - 2) {
    }

    std::string operator()() {
        size_t len = len_dis_(gen_);
        std::string res;
        res.reserve(len);
        std::generate_n(std::back_inserter(res), len, 
            [&]() { return chars_[char_dis_(gen_)];
            });
        return res;
    }

private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<size_t> len_dis_;
    std::uniform_int_distribution<> char_dis_;
    const char* chars_ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
};

class MonomialGenerator {
public:

    MonomialGenerator(double coeff_min = -100.0, double coeff_max = 100.0)
        : gen_(std::random_device{}())
        , coeff_dis_(coeff_min, coeff_max) {
    }

    polynomial::Monomial operator()() {
        size_t var_count = gen_() % 10;
        containers::Vector<polynomial::Variable> vars(var_count);
        std::generate_n(vars.begin(), var_count, 
            [this]() -> polynomial::Variable {
            uint8_t var_name = 'a' + (gen_() % 26);
            int32_t var_power = gen_() % 20;
            return { var_name, var_power };
            });
        return polynomial::Monomial(coeff_dis_(gen_), vars.begin(), vars.end());
    }

private:
    std::mt19937_64 gen_;
    std::uniform_real_distribution<> coeff_dis_;
};

class PolynomialGenerator {
public:

    PolynomialGenerator(size_t max_monomials = 250)
        : gen_(std::random_device{}())
        , monom_count_dis_(1, max_monomials)
        , monom_gen_(-100.0, 100.0) {
    }

    polynomial::Polynomial operator()() {
        size_t monom_count = monom_count_dis_(gen_);
        containers::Vector<polynomial::Monomial> monoms(monom_count);
        std::generate_n(monoms.begin(), monom_count, monom_gen_);
        return polynomial::Polynomial(monoms.begin(), monoms.end());
    }


private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<size_t> monom_count_dis_;
    MonomialGenerator monom_gen_;
};

}