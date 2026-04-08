#include "test_common_tables/test_common_tables.hpp"
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "polynomial/hash/monomial_hash.hpp"
#include "polynomial/hash/polynomial_hash.hpp"
#include "tables/hash_table.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using StringToString = tables::HashTable<std::string, std::string>;
using MonomialToMonomial = tables::HashTable<polynomial::Monomial, polynomial::Monomial>;
using PolynomialToPolynomial = tables::HashTable<polynomial::Polynomial, polynomial::Polynomial>;
using StringToInt = tables::HashTable<std::string, int>;
using MonomialToString = tables::HashTable<polynomial::Monomial, std::string>;
using PolynomialToDouble = tables::HashTable<polynomial::Polynomial, double>;
using IntToString = tables::HashTable<int, std::string>;
using StringToMonomial = tables::HashTable<std::string, polynomial::Monomial>;


using HashTableTypes = ::testing::Types<
    StringToString,
    MonomialToMonomial,
    PolynomialToPolynomial,
    StringToInt,
    MonomialToString,
    PolynomialToDouble,
    IntToString,
    StringToMonomial
>;

INSTANTIATE_TYPED_TEST_SUITE_P(Hash, TableTest, HashTableTypes);

}