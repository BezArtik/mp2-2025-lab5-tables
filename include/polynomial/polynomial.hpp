#pragma once
#include "containers/sorted_list.hpp"
#include "polynomial/monomial.hpp"

namespace polynomial {

class Polynomial {
public:
    Polynomial() = default;
    Polynomial(const Monomial& monom);
    template <typename Iter>
    Polynomial(Iter monoms_begin, Iter monoms_end) 
        : monomials_(monoms_begin, monoms_end) {
        combine_like_terms();
    }
    Polynomial(std::initializer_list<Monomial> init) 
        : Polynomial(init.begin(), init.end()) {
    }
    Polynomial(const std::string& str);

    Polynomial& operator+=(Monomial rhs);
    Polynomial& operator-=(Monomial rhs);
    Polynomial& operator+=(Polynomial rhs);
    Polynomial& operator-=(Polynomial rhs);
    Polynomial& operator*=(double scalar) noexcept;
    Polynomial& operator*=(const Monomial& rhs);
    Polynomial& operator*=(const Polynomial& rhs);

    auto begin()        noexcept { return monomials_.begin(); }
    auto end()          noexcept { return monomials_.end(); }
    auto begin()  const noexcept { return monomials_.cbegin(); }
    auto end()    const noexcept { return monomials_.cend(); }
    auto cbegin() const noexcept { return monomials_.cbegin(); }
    auto cend()   const noexcept { return monomials_.cend(); }

    bool is_zero() const noexcept { return monomials_.empty(); };
    size_t term_count() const noexcept { return monomials_.size(); };
    int32_t deg() const {
        if (monomials_.empty()) return 0;
        return monomials_.front().total_deg();
    }
    containers::SortedList<uint8_t> get_variables() const noexcept;

private:
    using monomials_t = containers::SortedList<Monomial, MonomialCompare>;
    monomials_t monomials_;

    void combine_like_terms() noexcept;
    void parse_from_string(const std::string& str);
};

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