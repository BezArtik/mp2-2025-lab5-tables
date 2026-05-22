#pragma once
#include "containers/sorted_list.hpp"

namespace polynomial {

struct Variable {
    uint8_t name_;
    int32_t power_;
};

struct VariableCompare {
    bool operator()(const Variable& v1, const Variable& v2) const noexcept;
};

class Monomial {
    using variables_t = containers::SortedList<Variable, VariableCompare>;
    using iterator = variables_t::iterator;
    using const_iterator = variables_t::const_iterator;
public:
    template <typename Iter>
    Monomial(double coeff, Iter start, Iter finish);
    Monomial(double coeff = 1.0, std::initializer_list<Variable> vars = {});
    Monomial(const std::string& str);
    
    Monomial& operator*=(double scalar) noexcept;
    Monomial& operator*=(const Monomial& rhs);
    void add_variable(const Variable& var);

    int32_t total_deg() const noexcept;
    bool is_similar(const Monomial& other) const noexcept;

    double coefficient() const noexcept;
    void set_coefficient(double coeff) noexcept;
    size_t variables_count() const noexcept;
    bool has_variables() const noexcept;
    bool is_zero() const noexcept;
    static bool is_zero(double scalar) noexcept;

    iterator       begin()        noexcept;
    iterator       end()          noexcept;
    const_iterator begin()  const noexcept;
    const_iterator end()    const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend()   const noexcept;

private:
    double coefficient_;
    variables_t variables_;

    void parse_from_string(const std::string& str);
    void normalize() noexcept;
};

struct MonomialCompare {
    bool operator()(const Monomial& lhs, const Monomial& rhs) const noexcept;
};

template <typename Iter>
Monomial::Monomial(double coeff, Iter begin, Iter end) : coefficient_(coeff) {
    for (; begin != end; ++begin) {
        if (begin->power_ < 0) {
            throw std::invalid_argument("Negative power in variable");
        }
        add_variable(*begin);
    }
}

Monomial operator*(double scalar, const Monomial& rhs) noexcept;
Monomial operator*(Monomial lhs, double scalar) noexcept;
Monomial operator*(Monomial lhs, const Monomial& rhs);

bool operator==(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator!=(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator<(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator>(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator<=(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator>=(const Monomial& lhs, const Monomial& rhs) noexcept;

std::ostream& operator<<(std::ostream& ostr, const Monomial& m);

}