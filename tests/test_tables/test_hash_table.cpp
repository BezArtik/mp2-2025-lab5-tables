#include "test_common/test_common_tables.hpp"
#include "polynomial/monomial.hpp"
#include "polynomial/polynomial.hpp"
#include "hash/monomial_hash.hpp"
#include "hash/polynomial_hash.hpp"
#include "tables/hash_table.hpp"
#include <gtest/gtest-typed-test.h>
#include <gtest/internal/gtest-type-util.h>
#include <string>

namespace tests {

using HashTableTypes = ::testing::Types<
	tables::HashTable<std::string, std::string>,
	tables::HashTable<polynomial::Monomial, polynomial::Monomial>,
	tables::HashTable<polynomial::Polynomial, polynomial::Polynomial>,
	tables::HashTable<std::string, int>,
	tables::HashTable<polynomial::Monomial, std::string>,
	tables::HashTable<polynomial::Polynomial, double>,
	tables::HashTable<int, std::string>,
	tables::HashTable<std::string, polynomial::Monomial>
>;

INSTANTIATE_TYPED_TEST_SUITE_P(Hash, TableTest, HashTableTypes);

}