#pragma once
#include "containers/sorted_list.hpp"
#include "polynomial/core/monomial.hpp"

namespace polynomial {

class Polynomial {
    using monomials_t = containers::SortedList<Monomial, MonomialCompare>;
    using iterator = monomials_t::iterator;
    using const_iterator = monomials_t::const_iterator;
public:
    Polynomial() = default;
    Polynomial(const Monomial& monom);
    template <typename Iter>
    Polynomial(Iter monoms_begin, Iter monoms_end);
    Polynomial(std::initializer_list<Monomial> init);
    Polynomial(const std::string& str);

    Polynomial& operator+=(Monomial rhs);
    Polynomial& operator-=(Monomial rhs);
    Polynomial& operator+=(Polynomial rhs);
    Polynomial& operator-=(Polynomial rhs);
    Polynomial& operator*=(double scalar) noexcept;
    Polynomial& operator*=(const Monomial& rhs);
    Polynomial& operator*=(const Polynomial& rhs);

    iterator begin()              noexcept;
    iterator end()                noexcept;
    const_iterator begin()  const noexcept;
    const_iterator end()    const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend()   const noexcept;

    containers::SortedList<uint8_t> get_variables() const noexcept;

    bool is_zero() const noexcept;
    size_t term_count() const noexcept;
    int32_t deg() const;

private:
    monomials_t monomials_;

    void combine_like_terms() noexcept;
    void parse_from_string(const std::string& str);
};

template <typename Iter>
Polynomial::Polynomial(Iter monoms_begin, Iter monoms_end)
    : monomials_(monoms_begin, monoms_end) {
    combine_like_terms();
}

Polynomial operator+(Polynomial lhs, Polynomial rhs);
Polynomial operator-(Polynomial lhs, Polynomial rhs);
Polynomial operator*(Polynomial lhs, double scalar) noexcept;
Polynomial operator*(double scalar, Polynomial rhs) noexcept;
Polynomial operator*(Polynomial lhs, Monomial rhs);
Polynomial operator*(Monomial lhs, Polynomial rhs);
Polynomial operator*(Polynomial lhs, Polynomial rhs);

bool operator==(const Polynomial& lhs, const Polynomial& rhs) noexcept;
bool operator!=(const Polynomial& lhs, const Polynomial& rhs) noexcept;
bool operator<(const Polynomial& lhs, const Polynomial& rhs) noexcept;
bool operator>(const Polynomial& lhs, const Polynomial& rhs) noexcept;
bool operator<=(const Polynomial& lhs, const Polynomial& rhs) noexcept;
bool operator>=(const Polynomial& lhs, const Polynomial& rhs) noexcept;

std::ostream& operator<<(std::ostream& ostr, const Polynomial& p);

}