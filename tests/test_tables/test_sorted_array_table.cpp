#include "test_common_tables/test_common_tables.hpp"
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "tables/sorted_array.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using StringToString = tables::SortedArrayTable<std::string, std::string>;
using MonomialToMonomial = tables::SortedArrayTable<polynomial::Monomial, polynomial::Monomial>;
using PolynomialToPolynomial = tables::SortedArrayTable<polynomial::Polynomial, polynomial::Polynomial>;
using StringToInt = tables::SortedArrayTable<std::string, int>;
using MonomialToString = tables::SortedArrayTable<polynomial::Monomial, std::string>;
using PolynomialToDouble = tables::SortedArrayTable<polynomial::Polynomial, double>;
using IntToString = tables::SortedArrayTable<int, std::string>;
using StringToMonomial = tables::SortedArrayTable<std::string, polynomial::Monomial>;


using SortedArrayTableTypes = ::testing::Types<
    StringToString,
    MonomialToMonomial,
    PolynomialToPolynomial,
    StringToInt,
    MonomialToString,
    PolynomialToDouble,
    IntToString,
    StringToMonomial
>;

INSTANTIATE_TYPED_TEST_SUITE_P(SortedArray, TableTest, SortedArrayTableTypes);

}