#pragma once
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include <functional>

namespace polynomial {

    struct MonomialHash {
        size_t operator()(const Monomial& m) const noexcept {
            size_t h = std::hash<double>{}(m.coefficient());

            for (const auto& var : m) {
                h ^= std::hash<uint8_t>{}(var.name_) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<int32_t>{}(var.power_) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }

            return h;
        }
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