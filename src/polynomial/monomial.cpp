#include "polynomial/monomial.hpp"
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

bool polynomial::MonomialCompare::operator()(const Monomial& lhs, const Monomial& rhs) const noexcept {
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

void polynomial::Monomial::add_variable(const Variable& var) {
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
    } else {
        variables_.insert(var);
    }
}

void polynomial::Monomial::parse_from_string(const std::string& str) {
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

void polynomial::Monomial::normalize() noexcept {
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
        } else {
            ++it;
        }
    }
}

polynomial::Monomial::Monomial(const std::string& str)
    : coefficient_(1.0) {
    parse_from_string(str);
}

int32_t polynomial::Monomial::total_deg() const noexcept {
    return std::accumulate(variables_.cbegin(), variables_.cend(), 0, 
        [](auto sum, const auto& var) {
        return sum + var.power_;
    });
}

bool polynomial::Monomial::is_similar(const Monomial& other) const noexcept {
    return variables_.size() == other.variables_.size() &&
           std::equal(variables_.cbegin(), variables_.cend(), other.variables_.cbegin(),
               [](const auto& v1, const auto& v2) {
                   return v1.name_ == v2.name_ && v1.power_ == v2.power_;
               });
}

polynomial::Monomial& polynomial::Monomial::operator*=(double scalar) noexcept {
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

polynomial::Monomial& polynomial::Monomial::operator*=(Monomial other) {
    if (variables_.empty() && other.variables_.empty()) {
        return *this;
    }
    *this *= other.coefficient_;
    variables_.merge(std::move(other.variables_));
    normalize();
    return *this;
}

polynomial::Monomial polynomial::operator*(Monomial other, double scalar) noexcept {
    return other *= scalar;
}

polynomial::Monomial polynomial::operator*(double scalar, const Monomial& other) noexcept {
    return other * scalar;
}

polynomial::Monomial polynomial::operator*(Monomial lhs, Monomial rhs) {
    return lhs *= std::move(rhs);
}

bool polynomial::operator==(const Monomial& lhs, const Monomial& rhs) noexcept {
    return (std::abs(lhs.coefficient() - rhs.coefficient()) < std::numeric_limits<double>::epsilon())
        && lhs.is_similar(rhs);
}

bool polynomial::operator!=(const Monomial& lhs, const Monomial& rhs) noexcept {
    return !(lhs == rhs);
}

bool polynomial::operator<(const Monomial& lhs, const Monomial& rhs) noexcept {
    return MonomialCompare{}(lhs, rhs);
}
bool polynomial::operator>(const Monomial& lhs, const Monomial& rhs) noexcept {
    return rhs < lhs;
}

bool polynomial::operator<=(const Monomial& lhs, const Monomial& rhs) noexcept {
    return !(rhs < lhs);
}

bool polynomial::operator>=(const Monomial& lhs, const Monomial& rhs) noexcept {
    return !(lhs < rhs);
}

std::ostream& polynomial::operator<<(std::ostream& ostr, const polynomial::Monomial& m) {
    if (m.is_zero()) {
        ostr << "0";
        return ostr;
    }

    if (m.coefficient() == -1.0 && m.has_variables()) {
        ostr << "-";
    } else if (!m.has_variables() || m.coefficient() != 1.0) {
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