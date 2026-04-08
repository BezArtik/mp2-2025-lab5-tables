#include "test_common_tables/test_common_tables.hpp"
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"
#include "tables/rb_tree.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using StringToString = tables::RBTreeTable<std::string, std::string>;
using MonomialToMonomial = tables::RBTreeTable<polynomial::Monomial, polynomial::Monomial>;
using PolynomialToPolynomial = tables::RBTreeTable<polynomial::Polynomial, polynomial::Polynomial>;
using StringToInt = tables::RBTreeTable<std::string, int>;
using MonomialToString = tables::RBTreeTable<polynomial::Monomial, std::string>;
using PolynomialToDouble = tables::RBTreeTable<polynomial::Polynomial, double>;
using IntToString = tables::RBTreeTable<int, std::string>;
using StringToMonomial = tables::RBTreeTable<std::string, polynomial::Monomial>;


using RBTreeTableTypes = ::testing::Types<
    StringToString,
    MonomialToMonomial,
    PolynomialToPolynomial,
    StringToInt,
    MonomialToString,
    PolynomialToDouble,
    IntToString,
    StringToMonomial
>;

INSTANTIATE_TYPED_TEST_SUITE_P(RBTree, TableTest, RBTreeTableTypes);

}