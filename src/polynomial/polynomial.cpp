#include "polynomial/core/polynomial.hpp"
#include "polynomial/core/monomial.hpp"
#include "containers/sorted_list.hpp"
#include "containers/vector.hpp"
#include <limits>
#include <string>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <cstdint>
#include <iterator>

void polynomial::Polynomial::combine_like_terms() noexcept {
    if (monomials_.empty()) return;

    auto it = monomials_.begin();
    auto end = monomials_.end();

    while(it != end) {
        auto next = std::next(it);
        while (next != end && it->is_similar(*next)) {
            it->set_coefficient(it->coefficient() + next->coefficient());
            next = monomials_.erase(next);
        }
        if (it->is_zero()) {
            it = monomials_.erase(it);
        } else {
            ++it;
        }
    }
}

polynomial::Polynomial::Polynomial(const polynomial::Monomial& monom) {
    if (!monom.is_zero()) {
        monomials_.insert(monom);
    }
}

void polynomial::Polynomial::parse_from_string(const std::string& str) {
    std::string s = str;
    std::erase_if(s, [](auto ch) {return std::isspace(ch); });
    size_t count = std::count_if(s.begin(), s.end(), 
        [](uint8_t c) { return c == '+' || c == '-'; });
    containers::Vector<std::string> temp;
    temp.reserve(count + 1);

    auto start = s.begin();
    auto end = s.end();
    while (start != end) {
        auto next = std::find_if(std::next(start), end, 
            [](uint8_t c) { return c == '+' || c == '-'; });
        temp.push_back(std::string(start, next));
        start = next;
    }
	monomials_ = std::move(monomials_t(temp.begin(), temp.end()));
}

polynomial::Polynomial::Polynomial(const std::string& str) {
    parse_from_string(str);
    combine_like_terms();
}

containers::SortedList<uint8_t> polynomial::Polynomial::get_variables() const noexcept {
    containers::SortedList<uint8_t> temp;
    for (const auto& monom : monomials_) {
        for (const auto& var : monom) {
            temp.insert_unique(var.name_);
        }
    }
    return temp;
}

polynomial::Polynomial& polynomial::Polynomial::operator+=(Monomial rhs) {
    if (rhs.is_zero()) return *this;
    monomials_.insert(std::move(rhs));
    combine_like_terms();
    return *this;
}

polynomial::Polynomial& polynomial::Polynomial::operator-=(Monomial rhs) {
    return *this += (-1.0) * std::move(rhs);
}

polynomial::Polynomial& polynomial::Polynomial::operator+=(Polynomial rhs) {
    if (rhs.is_zero()) return *this;
    monomials_.merge(std::move(rhs.monomials_));
    combine_like_terms();
    return *this;
}

polynomial::Polynomial& polynomial::Polynomial::operator-=(Polynomial rhs) {
    return *this += (-1.0) * std::move(rhs);
}

polynomial::Polynomial& polynomial::Polynomial::operator*=(double scalar) noexcept {
    constexpr double eps = std::numeric_limits<double>::epsilon();
    if (std::abs(scalar) < eps) {
        monomials_.clear();
        return *this;
    }
    if (std::abs(scalar - 1.0) > eps) {
		std::for_each(monomials_.begin(), monomials_.end(), 
            [scalar](auto& m) { m *= scalar; });
        combine_like_terms();
    }

    return *this;
}

polynomial::Polynomial& polynomial::Polynomial::operator*=(const Monomial& rhs) {
    if (rhs.is_zero()) {
        monomials_.clear();
        return *this;
    }
    std::for_each(monomials_.begin(), monomials_.end(), 
        [&rhs](auto& m) { m *= rhs; });
    combine_like_terms();
    return *this;
}

polynomial::Polynomial& polynomial::Polynomial::operator*=(const Polynomial& rhs) {
    if (rhs.is_zero()) {
        monomials_.clear();
        return *this;
    }
    containers::Vector<Monomial> temp;
    temp.reserve(monomials_.size() * rhs.monomials_.size());
    for (const auto& monom1 : monomials_) {
        for (const auto& monom2 : rhs.monomials_) {
            temp.push_back(monom1 * monom2);
        }
    }
    monomials_ = std::move(monomials_t(temp.begin(), temp.end()));
    combine_like_terms();
    return *this;
}

polynomial::Polynomial polynomial::operator+(Polynomial lhs, Polynomial rhs) {
    return lhs += std::move(rhs);
}

polynomial::Polynomial polynomial::operator-(Polynomial lhs, Polynomial rhs) {
    return lhs -= std::move(rhs);
}

polynomial::Polynomial polynomial::operator*(Polynomial lhs, double scalar) noexcept {
    return lhs *= scalar;
}

polynomial::Polynomial polynomial::operator*(double scalar, Polynomial rhs) noexcept {
    return rhs * scalar;
}

polynomial::Polynomial polynomial::operator*(Polynomial lhs, Monomial rhs) {
    return lhs *= std::move(rhs);
}

polynomial::Polynomial polynomial::operator*(Monomial lhs, Polynomial rhs) {
    return rhs *= std::move(lhs);
}

polynomial::Polynomial polynomial::operator*(Polynomial lhs, Polynomial rhs) {
    return lhs *= std::move(rhs);
}

bool polynomial::operator==(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return lhs.term_count() == rhs.term_count() && 
        std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

bool polynomial::operator!=(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return !(lhs == rhs);
}

bool polynomial::operator<(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end(),
        MonomialCompare{}
    );
}

bool polynomial::operator>(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return rhs < lhs;
}

bool polynomial::operator<=(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return !(rhs < lhs);
}

bool polynomial::operator>=(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return !(lhs < rhs);
}

std::ostream& polynomial::operator<<(std::ostream& ostr, const polynomial::Polynomial& p) {
    if (p.is_zero()) {
        return ostr << "0";
    }

    auto it = p.begin();
    ostr << *it;

    std::for_each(std::next(it), p.end(), 
        [&ostr](const auto& monom) {
        monom.coefficient() >= 0.0 ? ostr << " + " << monom : ostr << " - " << (-1.0) * monom;
        });

    return ostr;
}