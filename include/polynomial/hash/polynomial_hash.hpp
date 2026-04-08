#pragma once
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "polynomial/hash/monomial_hash.hpp"
#include <functional>

namespace polynomial {

    struct PolynomialHash {
        size_t operator()(const Polynomial& p) const noexcept {
            size_t h = 0;
            MonomialHash monomial_hasher;

            for (const auto& monom : p) {
                size_t monom_hash = monomial_hasher(monom);
                h ^= monom_hash + 0x9e3779b9 + (h << 6) + (h >> 2);
            }

            return h;
        }
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