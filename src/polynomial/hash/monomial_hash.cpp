#include "polynomial/core/monomial.hpp"
#include "polynomial/hash/monomial_hash.hpp"
#include <type_traits>
#include <cstdint>

namespace polynomial {

size_t MonomialHash::operator()(const Monomial& m) const noexcept {
    size_t h = std::hash<double>{}(m.coefficient());

    for (const auto& var : m) {
        h ^= std::hash<uint8_t>{}(var.name_) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<int32_t>{}(var.power_) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }

    return h;
}

}