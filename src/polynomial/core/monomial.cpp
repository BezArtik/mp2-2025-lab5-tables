#include "polynomial/core/monomial.hpp"
#include "containers/sorted_list.hpp"
#include <limits>
#include <string>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <cstdlib>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <initializer_list>

namespace polynomial {

void Monomial::parse_from_string(const std::string& str) {
    if (str.empty()) {
        throw std::invalid_argument("Empty string");
    }

    std::string s = str;
    std::erase_if(s, [](auto ch) {return std::isspace(ch); });

    auto it = s.cbegin();
    auto end = s.cend();

    bool negative = false;
    if (it != end && (*it == '+' || *it == '-')) {
        negative = (*it == '-');
        ++it;
    }

    bool has_explicit_coefficient = false;
    if (it != end && (std::isdigit(*it) || *it == '.')) {
        auto start = it;
        it = std::find_if(it, end,
            [](uint8_t c) { return !std::isdigit(c) && c != '.'; });
        coefficient_ = std::stod(std::string(start, it));
        has_explicit_coefficient = true;
    }

    if (negative) {
        coefficient_ = -coefficient_;
    }

    while (it != end) {
        if (*it == '*') {
            ++it;
            continue;
        }
        if (!std::isalpha(*it)) {
            throw std::invalid_argument("Invalid variable name");
        }
        uint8_t var_name = *it;
        ++it;
        int32_t power = 1;
        if (it != end && *it == '^') {
            ++it;
            if (it == end || !std::isdigit(*it)) {
                throw std::invalid_argument("Invalid power after '^'");
            }
            auto power_start = it;
            it = std::find_if(it, end, [](uint8_t c) { return !std::isdigit(c); });
            power = std::stoi(std::string(power_start, it));
        }
        add_variable({ var_name, power });
    }


    if (is_zero()) {
        variables_.clear();
    }

    if (!has_explicit_coefficient && variables_.empty()) {
        throw std::invalid_argument("Invalid monom format");
    }
}

void Monomial::normalize() noexcept {
    if (is_zero()) {
        variables_.clear();
        return;
    }
    auto it = variables_.begin();
    auto end = variables_.end();
    while (it != end) {
        auto next = std::next(it);
        while (next != end && next->name_ == it->name_) {
            it->power_ += next->power_;
            next = variables_.erase(next);
        }
        if (it->power_ == 0) {
            it = variables_.erase(it);
        }
        else {
            ++it;
        }
    }
}


Monomial::Monomial::Monomial(double coeff, std::initializer_list<Variable> vars)
    : Monomial(coeff, vars.begin(), vars.end()) {
}

Monomial::Monomial(const std::string& str)
    : coefficient_(1.0) {
    parse_from_string(str);
}

void Monomial::add_variable(const Variable& var) {
    if (var.power_ == 0) return;

    auto it = std::find_if(variables_.begin(), variables_.end(),
        [&var](const Variable& v) { return v.name_ == var.name_; });

    if (it != variables_.end()) {
        Variable updated = *it;
        updated.power_ += var.power_;
        variables_.erase(it);
        if (updated.power_ != 0) {
            variables_.insert(updated);
        }
    }
    else {
        variables_.insert(var);
    }
}

int32_t Monomial::total_deg() const noexcept {
    return std::accumulate(variables_.cbegin(), variables_.cend(), 0,
        [](auto sum, const auto& var) {
            return sum + var.power_;
        });
}

bool Monomial::is_similar(const Monomial& other) const noexcept {
    return variables_.size() == other.variables_.size() &&
        std::equal(variables_.cbegin(), variables_.cend(), other.variables_.cbegin(),
            [](const auto& v1, const auto& v2) {
                return v1.name_ == v2.name_ && v1.power_ == v2.power_;
            });
}

double Monomial::coefficient() const noexcept { return coefficient_; }
void Monomial::set_coefficient(double coeff) noexcept { coefficient_ = coeff; }
size_t Monomial::variables_count() const noexcept { return variables_.size(); }
bool Monomial::has_variables() const noexcept { return !variables_.empty(); }
bool Monomial::is_zero() const noexcept {
    return std::abs(coefficient_) < std::numeric_limits<double>::epsilon();
}

Monomial::iterator       Monomial::begin()        noexcept { return variables_.begin(); }
Monomial::iterator       Monomial::end()          noexcept { return variables_.end(); }
Monomial::const_iterator Monomial::begin()  const noexcept { return variables_.cbegin(); }
Monomial::const_iterator Monomial::end()    const noexcept { return variables_.cend(); }
Monomial::const_iterator Monomial::cbegin() const noexcept { return variables_.cbegin(); }
Monomial::const_iterator Monomial::cend()   const noexcept { return variables_.cend(); }

bool VariableCompare::operator()(const Variable& v1, const Variable& v2) const noexcept {
    if (v1.name_ != v2.name_) return v1.name_ < v2.name_;
    return v1.power_ < v2.power_;
}

bool MonomialCompare::operator()(const Monomial& lhs, const Monomial& rhs) const noexcept {
    bool lhs_const = !lhs.has_variables();
    bool rhs_const = !rhs.has_variables();
    if (lhs_const != rhs_const) return rhs_const;

    if (lhs_const && rhs_const) return lhs.coefficient() < rhs.coefficient();

    if (lhs.total_deg() != rhs.total_deg()) {
        return lhs.total_deg() > rhs.total_deg();
    }

    auto lhs_it = lhs.cbegin();
    auto rhs_it = rhs.cbegin();
    while (lhs_it != lhs.cend() && rhs_it != rhs.cend()) {
        if (lhs_it->name_ != rhs_it->name_) return lhs_it->name_ < rhs_it->name_;
        if (lhs_it->power_ != rhs_it->power_) return lhs_it->power_ < rhs_it->power_;
        ++lhs_it;
        ++rhs_it;
    }
    if (lhs_it != lhs.cend() || rhs_it != rhs.cend()) {
        return lhs_it == lhs.cend();
    }

    return lhs.coefficient() < rhs.coefficient();
}

Monomial& Monomial::operator*=(double scalar) noexcept {
    if (std::abs(scalar) < std::numeric_limits<double>::epsilon()) {
        variables_.clear();
        return *this;
    }

    if (std::abs(scalar - 1.0) < std::numeric_limits<double>::epsilon()) {
        return *this;
    }
    coefficient_ *= scalar;
    return *this;
}

Monomial& Monomial::operator*=(Monomial other) {
    if (variables_.empty() && other.variables_.empty()) {
        return *this;
    }
    *this *= other.coefficient_;
    variables_.merge(std::move(other.variables_));
    normalize();
    return *this;
}

Monomial operator*(Monomial other, double scalar) noexcept {
    return other *= scalar;
}

Monomial operator*(double scalar, const Monomial& other) noexcept {
    return other * scalar;
}

Monomial operator*(Monomial lhs, Monomial rhs) {
    return lhs *= std::move(rhs);
}

bool operator==(const Monomial& lhs, const Monomial& rhs) noexcept {
    return (std::abs(lhs.coefficient() - rhs.coefficient()) < std::numeric_limits<double>::epsilon())
        && lhs.is_similar(rhs);
}

bool operator!=(const Monomial& lhs, const Monomial& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator<(const Monomial& lhs, const Monomial& rhs) noexcept {
    return MonomialCompare{}(lhs, rhs);
}
bool operator>(const Monomial& lhs, const Monomial& rhs) noexcept {
    return rhs < lhs;
}

bool operator<=(const Monomial& lhs, const Monomial& rhs) noexcept {
    return !(rhs < lhs);
}

bool operator>=(const Monomial& lhs, const Monomial& rhs) noexcept {
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& ostr, const Monomial& m) {
    if (m.is_zero()) {
        ostr << "0";
        return ostr;
    }

    if (m.coefficient() == -1.0 && m.has_variables()) {
        ostr << "-";
    }
    else if (!m.has_variables() || m.coefficient() != 1.0) {
        ostr << m.coefficient();
    }

    for (const auto& var : m) {
        ostr << var.name_;
        if (var.power_ != 1) {
            ostr << "^" << var.power_;
        }
    }
    return ostr;
}

}