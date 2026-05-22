#include "generators/generators.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include "containers/vector.hpp"
#include <random>
#include <string>
#include <algorithm>
#include <cstdint>
#include <iterator>

namespace gen {

StringGenerator::StringGenerator(size_t min_len, size_t max_len)
    : gen_(std::random_device{}())
    , len_dis_(min_len, max_len)
    , char_dis_(0, sizeof(chars_) - 2) {
}

std::string StringGenerator::operator()() {
    size_t len = len_dis_(gen_);
    std::string res;
    res.reserve(len);
    std::generate_n(std::back_inserter(res), len,
        [&]() { return chars_[char_dis_(gen_)];
        });
    return res;
}


MonomialGenerator::MonomialGenerator(double coeff_min, double coeff_max)
    : gen_(std::random_device{}())
    , coeff_dis_(coeff_min, coeff_max) {
}

polynomial::Monomial MonomialGenerator::MonomialGenerator::operator()() {
    containers::Vector<polynomial::Variable> vars(gen_() % 10);
    std::generate(vars.begin(), vars.end(),
        [this]() -> polynomial::Variable {
            uint8_t var_name = 'a' + (gen_() % 26);
            int32_t var_power = gen_() % 20;
            return { var_name, var_power };
        });
    return polynomial::Monomial(coeff_dis_(gen_), vars.begin(), vars.end());
}

PolynomialGenerator::PolynomialGenerator(size_t max_monomials)
    : gen_(std::random_device{}())
    , monom_count_dis_(1, max_monomials)
    , monom_gen_(-100.0, 100.0) {
}

polynomial::Polynomial PolynomialGenerator::operator()() {
    containers::Vector<polynomial::Monomial> monoms(monom_count_dis_(gen_));
    std::generate(monoms.begin(), monoms.end(), monom_gen_);
    return polynomial::Polynomial(monoms.begin(), monoms.end());
}

}