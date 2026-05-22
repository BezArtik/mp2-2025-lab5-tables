#include "test_common/test_common_tables.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include "tables/rb_tree.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using RBTreeTableTypes = ::testing::Types<
	tables::RBTreeTable<std::string, std::string>,
	tables::RBTreeTable<polynomial::Monomial, polynomial::Monomial>,
	tables::RBTreeTable<polynomial::Polynomial, polynomial::Polynomial>,
	tables::RBTreeTable<std::string, int>,
	tables::RBTreeTable<polynomial::Monomial, std::string>,
	tables::RBTreeTable<polynomial::Polynomial, double>,
	tables::RBTreeTable<int, std::string>,
	tables::RBTreeTable<std::string, polynomial::Monomial>
>;

INSTANTIATE_TYPED_TEST_SUITE_P(RBTree, TableTest, RBTreeTableTypes);

}