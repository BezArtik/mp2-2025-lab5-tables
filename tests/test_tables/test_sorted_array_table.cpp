#include "test_common/test_common_tables.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include "tables/sorted_array.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using SortedArrayTableTypes = ::testing::Types<
	tables::SortedArrayTable<std::string, std::string>,
	tables::SortedArrayTable<polynomial::Monomial, polynomial::Monomial>,
	tables::SortedArrayTable<polynomial::Polynomial, polynomial::Polynomial>,
	tables::SortedArrayTable<std::string, int>,
	tables::SortedArrayTable<polynomial::Monomial, std::string>,
	tables::SortedArrayTable<polynomial::Polynomial, double>,
	tables::SortedArrayTable<int, std::string>,
	tables::SortedArrayTable<std::string, polynomial::Monomial>
>;

INSTANTIATE_TYPED_TEST_SUITE_P(SortedArray, TableTest, SortedArrayTableTypes);

}