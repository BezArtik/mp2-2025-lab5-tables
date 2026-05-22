#include "test_common/test_common_tables.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include "tables/unsorted_array.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using UnsortedArrayTableTypes = ::testing::Types<
	tables::UnsortedArrayTable<std::string, std::string>,
	tables::UnsortedArrayTable<polynomial::Monomial, polynomial::Monomial>,
	tables::UnsortedArrayTable<polynomial::Polynomial, polynomial::Polynomial>,
	tables::UnsortedArrayTable<std::string, int>,
	tables::UnsortedArrayTable<polynomial::Monomial, std::string>,
	tables::UnsortedArrayTable<polynomial::Polynomial, double>,
	tables::UnsortedArrayTable<int, std::string>,
	tables::UnsortedArrayTable<std::string, polynomial::Monomial>
>;

INSTANTIATE_TYPED_TEST_SUITE_P(UnsortedArray, TableTest, UnsortedArrayTableTypes);

}