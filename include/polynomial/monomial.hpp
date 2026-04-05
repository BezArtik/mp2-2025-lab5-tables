#pragma once
#include "containers/sorted_list.hpp"

namespace polynomial {

struct Variable {
    uint8_t name_;
    int32_t power_;
};

struct VariableValue {
    uint8_t name_;
    double value_;
};

struct VariableCompare {
    bool operator()(const Variable& v1, const Variable& v2) const noexcept {
        if (v1.name_ != v2.name_) return v1.name_ < v2.name_;
        return v1.power_ < v2.power_;
    }
};

struct VariableValueCompare {
    bool operator()(const VariableValue& v1, const VariableValue& v2) const noexcept {
        return v1.name_ < v2.name_;
    }
};

class Monomial {
    using variables_t = containers::SortedList<Variable, VariableCompare>;
public:
    template <typename Iter>
    Monomial(double coeff, Iter vars_begin, Iter vars_end) : coefficient_(coeff) {
        for (auto it = vars_begin; it != vars_end; ++it) {
            if (it->power_ < 0) {
                throw std::invalid_argument("Negative power in variable");
            }
            add_variable(*it);
        }
    }
    Monomial(double coeff = 1.0, std::initializer_list<Variable> vars = {}) 
        : Monomial(coeff, vars.begin(), vars.end()) {}
    Monomial(const std::string& str);
    
    Monomial& operator*=(double scalar) noexcept;
    Monomial& operator*=(Monomial other);
    void add_variable(const Variable& var);

    auto begin()        noexcept { return variables_.begin(); }
    auto end()          noexcept { return variables_.end(); }
    auto begin()  const noexcept { return variables_.cbegin(); }
    auto end()    const noexcept { return variables_.cend(); }
    auto cbegin() const noexcept { return variables_.cbegin(); }
    auto cend()   const noexcept { return variables_.cend(); }

    double coefficient() const noexcept { return coefficient_; }
    void set_coefficient(double coeff) noexcept { coefficient_ = coeff; }

    int32_t total_deg() const noexcept;
    size_t variables_count() const noexcept { return variables_.size(); }
    bool is_similar(const Monomial& other) const noexcept;
    bool has_variables() const noexcept { return !variables_.empty(); }
    bool is_zero() const noexcept { return std::abs(coefficient_) < std::numeric_limits<double>::epsilon(); }

private:
    double coefficient_;
    variables_t variables_;

    void parse_from_string(const std::string& str);
    void normalize() noexcept;
};

struct MonomialCompare {
    bool operator()(const Monomial& lhs, const Monomial& rhs) const noexcept;

};

Monomial operator*(double scalar, const Monomial& other) noexcept;
Monomial operator*(Monomial other, double scalar) noexcept;
Monomial operator*(Monomial lhs, Monomial rhs) noexcept;

bool operator==(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator!=(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator<(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator>(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator<=(const Monomial& lhs, const Monomial& rhs) noexcept;
bool operator>=(const Monomial& lhs, const Monomial& rhs) noexcept;

std::ostream& operator<<(std::ostream& ostr, const Monomial& m);

}