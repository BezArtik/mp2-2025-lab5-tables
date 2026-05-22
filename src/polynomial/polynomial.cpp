#include "polynomial/polynomial.hpp"
#include "polynomial/monomial.hpp"
#include "containers/sorted_list.hpp"
#include "containers/vector.hpp"
#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <utility>
#include <cstdint>
#include <iterator>
#include <initializer_list>

namespace polynomial {

void Polynomial::parse_from_string(const std::string& str) {
    std::string s = str;
    std::erase_if(s, [](auto ch) {return std::isspace(ch); });
    auto count = std::count_if(s.begin(), s.end(),
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

void Polynomial::combine_like_terms() noexcept {
    if (monomials_.empty()) return;

    auto it = monomials_.begin();
    auto end = monomials_.end();

    while (it != end) {
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

Polynomial::Polynomial(const Monomial& monom) {
    if (!monom.is_zero()) {
        monomials_.insert(monom);
    }
}

Polynomial::Polynomial(std::initializer_list<Monomial> init)
    : Polynomial(init.begin(), init.end()) {
}

Polynomial::Polynomial(const std::string& str) {
    parse_from_string(str);
    combine_like_terms();
}

Polynomial::iterator       Polynomial::begin()        noexcept { return monomials_.begin(); }
Polynomial::iterator       Polynomial::end()          noexcept { return monomials_.end(); }
Polynomial::const_iterator Polynomial::begin()  const noexcept { return monomials_.cbegin(); }
Polynomial::const_iterator Polynomial::end()    const noexcept { return monomials_.cend(); }
Polynomial::const_iterator Polynomial::cbegin() const noexcept { return monomials_.cbegin(); }
Polynomial::const_iterator Polynomial::cend()   const noexcept { return monomials_.cend(); }

containers::SortedList<uint8_t> Polynomial::get_variables() const noexcept {
    containers::SortedList<uint8_t> temp;
    for (const auto& monom : monomials_) {
        for (const auto& var : monom) {
            temp.insert_unique(var.name_);
        }
    }
    return temp;
}

bool Polynomial::is_zero() const noexcept { return monomials_.empty(); };
size_t Polynomial::term_count() const noexcept { return monomials_.size(); };
int32_t Polynomial::deg() const {
    if (monomials_.empty()) return 0;
    return monomials_.front().total_deg();
}

Polynomial& Polynomial::operator+=(const Monomial& rhs) {
    if (rhs.is_zero()) return *this;
    monomials_.insert(rhs);
    combine_like_terms();
    return *this;
}

Polynomial& Polynomial::operator-=(const Monomial& rhs) {
    return *this += (-1.0) * rhs;
}

Polynomial& Polynomial::operator+=(const Polynomial& rhs) {
    if (rhs.is_zero()) return *this;
    auto temp = rhs;
    monomials_.merge(std::move(temp.monomials_));
    combine_like_terms();
    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& rhs) {
    return *this += (-1.0) * rhs;
}

Polynomial& Polynomial::operator*=(double scalar) noexcept {
    if (Monomial::is_zero(scalar)) {
        monomials_.clear();
        return *this;
    }
    if (Monomial::is_zero(scalar - 1.0)) {
        return *this;
    }
    std::for_each(monomials_.begin(), monomials_.end(),
        [scalar](auto& m) { m *= scalar; });
    return *this;
}

Polynomial& Polynomial::operator*=(const Monomial& rhs) {
    if (rhs.is_zero()) {
        monomials_.clear();
        return *this;
    }
    std::for_each(monomials_.begin(), monomials_.end(),
        [&rhs](auto& m) { m *= rhs; });
    combine_like_terms();
    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial& rhs) {
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

Polynomial operator+(Polynomial lhs, const Polynomial& rhs) {
    return lhs += rhs; 
}

Polynomial operator-(Polynomial lhs, const Polynomial& rhs) {
    return lhs -= rhs;
}

Polynomial operator*(Polynomial lhs, double scalar) noexcept {
    return lhs *= scalar;
}

Polynomial operator*(double scalar, Polynomial rhs) noexcept {
    return rhs *= scalar;
}

Polynomial operator*(Polynomial lhs, const Monomial& rhs) {
    return lhs *= rhs;
}

Polynomial operator*(Monomial lhs, const Polynomial& rhs) {
    return rhs * std::move(lhs);
}

Polynomial operator*(Polynomial lhs, const Polynomial& rhs) {
    return lhs *= rhs;
}

bool operator==(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return lhs.term_count() == rhs.term_count() &&
        std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

bool operator!=(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator<(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end(),
        MonomialCompare{}
    );
}

bool operator>(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return rhs < lhs;
}

bool operator<=(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return !(rhs < lhs);
}

bool operator>=(const Polynomial& lhs, const Polynomial& rhs) noexcept {
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& ostr, const Polynomial& p) {
    if (p.is_zero()) {
        return ostr << "0";
    }

    auto it = p.begin();
    ostr << *it;

    std::for_each(std::next(it), p.end(),
        [&ostr](const auto& monom) {
            monom.coefficient() >= 0.0 ? 
                ostr << " + " << monom : ostr << " - " << (-1.0) * monom;
        });

    return ostr;
}

}