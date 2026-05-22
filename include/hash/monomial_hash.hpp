#pragma once
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include <type_traits>

namespace polynomial {

struct MonomialHash {
    size_t operator()(const Monomial& m) const noexcept;
};

}

namespace std {

template<>
struct hash<polynomial::Monomial> {
    size_t operator()(const polynomial::Monomial& m) const noexcept {
        return polynomial::MonomialHash{}(m);
    }
};

}