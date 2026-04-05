#include "test_common.hpp"
#include "containers/vector.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include <random>
#include <string>
#include <algorithm>
#include <cstdint>
#include <iterator>

namespace tests {

size_t Tracker::copy_count_ = 0;
size_t Tracker::move_count_ = 0;
size_t Tracker::default_count_ = 0;
size_t Tracker::destruct_count_ = 0;

Tracker::Tracker() : value_(0) { ++default_count_; }
Tracker::Tracker(size_t v) : value_(v) { ++default_count_; }
Tracker::Tracker(const Tracker& other) : value_(other.value_) { ++copy_count_; }
Tracker::Tracker(Tracker&& other) noexcept : value_(std::exchange(other.value_, 0)) { ++move_count_; }
Tracker::~Tracker() { ++destruct_count_; }

Tracker& Tracker::operator=(const Tracker& other) {
    if (this != &other) {
        value_ = other.value_;
        ++copy_count_;
    }
    return *this;
}

Tracker& Tracker::operator=(Tracker&& other) noexcept {
    if (this != &other) {
        value_ = std::exchange(other.value_, 0);
        ++move_count_;
    }
    return *this;
}

bool Tracker::operator==(const Tracker& other) const noexcept { return value_ == other.value_; }
bool Tracker::operator<(const Tracker& other) const noexcept { return value_ < other.value_; }

void Tracker::reset_counts() {
    copy_count_ = move_count_ = default_count_ = destruct_count_ = 0;
}


const char* RandomStringGenerator::chars_ =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

RandomStringGenerator::RandomStringGenerator(size_t min_len, size_t max_len)
    : gen_(std::random_device{}())
    , len_dis_(min_len, max_len)
    , char_dis_(0, sizeof(chars_) - 2) {
}

std::string RandomStringGenerator::operator()() {
    size_t len = len_dis_(gen_);
    std::string res;
    res.reserve(len);
    std::generate_n(std::back_inserter(res), len, [&]() {
        return chars_[char_dis_(gen_)];
        });
    return res;
}

RandomMonomialGenerator::RandomMonomialGenerator(double coeff_min, double coeff_max)
    : gen_(std::random_device{}())
    , coeff_dis_(coeff_min, coeff_max) {
}

polynomial::Monomial RandomMonomialGenerator::operator()() {
    size_t var_count = gen_() % 10;
    containers::Vector<polynomial::Variable> vars(var_count);
    std::generate_n(vars.begin(), var_count, [this]() -> polynomial::Variable {
        uint8_t var_name = 'a' + (gen_() % 26);
        int32_t var_power = gen_() % 20;
        return { var_name, var_power };
        });
    return polynomial::Monomial(coeff_dis_(gen_), vars.begin(), vars.end());
}

RandomPolynomialGenerator::RandomPolynomialGenerator(size_t max_monomials)
    : gen_(std::random_device{}())
    , monom_count_dis_(1, max_monomials)
    , monom_gen_(-100.0, 100.0) {
}

polynomial::Polynomial RandomPolynomialGenerator::operator()() {
    size_t monom_count = monom_count_dis_(gen_);
    containers::Vector<polynomial::Monomial> monoms(monom_count);
    std::generate_n(monoms.begin(), monom_count, monom_gen_);
    return polynomial::Polynomial(monoms.begin(), monoms.end());
}

}