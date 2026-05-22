#pragma once
#include "polynomial/polynomial.hpp"
#include <type_traits>

namespace polynomial {

struct PolynomialHash {
    size_t operator()(const Polynomial& p) const noexcept;
};

} 

namespace std {

template<>
struct hash<polynomial::Polynomial> {
    size_t operator()(const polynomial::Polynomial& p) const noexcept {
        return polynomial::PolynomialHash{}(p);
    }
};

}