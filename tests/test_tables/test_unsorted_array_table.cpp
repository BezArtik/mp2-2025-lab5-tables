#include "test_common_tables/test_common_tables.hpp"
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "tables/unsorted_array.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using StringToString = tables::UnsortedArrayTable<std::string, std::string>;
using MonomialToMonomial = tables::UnsortedArrayTable<polynomial::Monomial, polynomial::Monomial>;
using PolynomialToPolynomial = tables::UnsortedArrayTable<polynomial::Polynomial, polynomial::Polynomial>;
using StringToInt = tables::UnsortedArrayTable<std::string, int>;
using MonomialToString = tables::UnsortedArrayTable<polynomial::Monomial, std::string>;
using PolynomialToDouble = tables::UnsortedArrayTable<polynomial::Polynomial, double>;
using IntToString = tables::UnsortedArrayTable<int, std::string>;
using StringToMonomial = tables::UnsortedArrayTable<std::string, polynomial::Monomial>;

using UnsortedArrayTableTypes = ::testing::Types<
    StringToString,
    MonomialToMonomial,
    PolynomialToPolynomial,
    StringToInt,
    MonomialToString,
    PolynomialToDouble,
    IntToString,
    StringToMonomial
>;

INSTANTIATE_TYPED_TEST_SUITE_P(UnsortedArray, TableTest, UnsortedArrayTableTypes);

}

