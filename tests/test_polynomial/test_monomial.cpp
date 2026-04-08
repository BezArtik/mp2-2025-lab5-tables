#include <gtest/gtest.h>
#include "polynomial/core/monomial.hpp"



TEST(Monomial, monom_with_coeff_has_no_variables) {
	polynomial::Monomial m(2.5);
	EXPECT_FALSE(m.has_variables());
}

TEST(Monomial, monom_with_coeff_and_variables_has_coeff_and_variables) {
	polynomial::Monomial m(2.5, { {'x',1},{'y',1} });
	EXPECT_DOUBLE_EQ(m.coefficient(), 2.5);
	auto it = m.cbegin();
	EXPECT_EQ(it->name_, 'x');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'y');
	EXPECT_EQ(it->power_, 1);
}

TEST(Monomial, monom_from_string_with_coeff_and_variables_has_coeff_and_variables) {
	polynomial::Monomial m("2.5xy");
	EXPECT_DOUBLE_EQ(m.coefficient(), 2.5);
	auto it = m.cbegin();
	EXPECT_EQ(it->name_, 'x');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'y');
	EXPECT_EQ(it->power_, 1);
}

TEST(Monomial, monom_sorts_variables) {
	polynomial::Monomial m("2.5dcab");
	auto it = m.cbegin();
	EXPECT_EQ(it->name_, 'a');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'b');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'c');
	EXPECT_EQ(it->power_, 1);
	++it;
	EXPECT_EQ(it->name_, 'd');
	EXPECT_EQ(it->power_, 1);
}

TEST(Monomial, equal_monomials_is_equal) {
	polynomial::Monomial m1("2.5xzy");
	polynomial::Monomial m2("2.5xyz");
	EXPECT_EQ(m1, m2);
}

TEST(Monomial, non_equal_monomials_is_not_equal) {
	polynomial::Monomial m1("5xzy^2");
	polynomial::Monomial m2("2.5xyz");
	EXPECT_NE(m1, m2);
}

TEST(Monomial, setting_the_coeff_sets_the_coeff) {
	polynomial::Monomial m;
	m.set_coefficient(2.5);
	EXPECT_DOUBLE_EQ(m.coefficient(), 2.5);
}

TEST(Monomial, adding_the_var_adds_the_var) {
	polynomial::Monomial m;
	m.add_variable({ 'x',2 });
	EXPECT_EQ(m.variables_count(), 1);
	EXPECT_EQ(m.total_deg(), 2);
	auto it = m.begin();
	EXPECT_EQ(it->name_, 'x');
	EXPECT_EQ(it->power_, 2);
}

TEST(Monomial, similar_monomials_is_similar) {
	polynomial::Monomial m1("2xz^3q^7");
	polynomial::Monomial m2("6z^3q^7x");
	EXPECT_TRUE(m1.is_similar(m2));
}


TEST(Monomial, can_mul_monom_by_scalar_with_assignment) {
	polynomial::Monomial m("2xy");
	m *= 2.5;
	EXPECT_DOUBLE_EQ(m.coefficient(), 5.0);
}

TEST(Monomial, can_mul_monom_by_monom_with_assignment) {
	polynomial::Monomial m("2xy");
	m *= {2.5, { {'y',1},{'x',1} }};
	polynomial::Monomial m1("5x^2y^2");
	EXPECT_EQ(m, m1);
}

TEST(Monomial, can_mul_monom_by_scalar) {
	polynomial::Monomial m("2xy");
	polynomial::Monomial m1 = m * 2.5;
	EXPECT_DOUBLE_EQ(m1.coefficient(), 5.0);
}

TEST(Monomial, can_mul_monomials) {
	polynomial::Monomial m1("yx");
	polynomial::Monomial m2("4w^2x");
	polynomial::Monomial m3("4yx^2w^2");

	EXPECT_EQ(m1 * m2, m3);
}

TEST(Monomial, mul_monom_by_scalar_is_commutative) {
	polynomial::Monomial m("2yzx");
	EXPECT_EQ(m * 2.5, 2.5 * m);
}