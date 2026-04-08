#include <gtest/gtest.h>
#include "polynomial/core/monomial.hpp"
#include "polynomial/core/polynomial.hpp"

TEST(Polynomial, empty_poly_has_zero_deg) {
	polynomial::Polynomial p;
	EXPECT_EQ(p.deg(), 0);
}

TEST(Polynomial, poly_with_monom_has_non_zero_deg) {
	polynomial::Polynomial p("2xyq");
	EXPECT_EQ(p.deg(), 3);
}

TEST(Polynomial, poly_sorts_monomials) {
	polynomial::Polynomial p("3abc - ab^2c + xy");
	auto it = p.begin();
	polynomial::Monomial m1(-1.0, { {'a',1},{'b',2},{'c',1} });
	polynomial::Monomial m2(3.0, { {'a',1},{'b',1},{'c',1} });
	polynomial::Monomial m3(1.0, { {'x',1},{'y',1} });
	EXPECT_EQ(*it, m1);
	++it;
	EXPECT_EQ(*it, m2);
	++it;
	EXPECT_EQ(*it, m3);
}

TEST(Polynomial, similar_polynomials_is_equal) {
	polynomial::Polynomial p1("3abc - 2bca + xyz");
	polynomial::Polynomial p2("abc + xyz");
	EXPECT_EQ(p1, p2);
}

TEST(Polynomial, non_similar_polynomials_is_not_equal) {
	polynomial::Polynomial p1("3abc - 15bca + xyz");
	polynomial::Polynomial p2("abc + xyz");
	EXPECT_NE(p1, p2);
}

TEST(Polynomial, polynomial_is_normalize_automatically) {
	polynomial::Polynomial p1("3a^2bc^2 - 2bc^2a^2 + xyz - 4yzx");
	EXPECT_EQ(p1.term_count(), 2);
}

TEST(Polynomial, can_add_monom_with_poly_with_assignment) {
	polynomial::Polynomial p("2xyz");
	polynomial::Monomial m("xyz^2");
	p += m;
	EXPECT_EQ(p, polynomial::Polynomial{ "xyz^2 + 2xyz" });
}

TEST(Polynomial, can_sub_monom_with_poly_with_assignment) {
	polynomial::Polynomial p("2xyz");
	polynomial::Monomial m("xyz^2");
	p -= m;
	EXPECT_EQ(p, polynomial::Polynomial{ "-xyz^2 + 2xyz" });
}

TEST(Polynomial, can_add_polynomials_with_assignment) {
	polynomial::Polynomial p1("2xyzw - 4t^2yz");
	polynomial::Polynomial p2("5t^2yz - xyzw");
	p1 += p2;
	EXPECT_EQ(p1, polynomial::Polynomial{ "t^2yz + xyzw" });
}

TEST(Polynomial, can_sub_polynomials_with_assignment) {
	polynomial::Polynomial p1("2xyzw - 4t^2yz");
	polynomial::Polynomial p2("5t^2yz - xyzw");
	p1 -= p2;
	EXPECT_EQ(p1, polynomial::Polynomial{ "-9t^2yz + 3xyzw" });
}

TEST(Polynomial, can_mul_poly_by_scalar_with_assignment) {
	polynomial::Polynomial p("2xyzq^2 - 5z^2y^3");
	p *= 5.0;
	EXPECT_EQ(p, polynomial::Polynomial{ "10xyzq^2 - 25z^2y^3" });
}

TEST(Polynomial, can_mul_poly_by_monom_with_assignment) {
	polynomial::Polynomial p("xyz - xy^2z^2");
	polynomial::Monomial m("x^2");
	p *= m;
	EXPECT_EQ(p, polynomial::Polynomial{ "x^3yz - x^3y^2z^2" });
}

TEST(Polynomial, can_mul_poly_by_poly_with_assignment) {
	polynomial::Polynomial p1("a^2 - b^2");
	polynomial::Polynomial p2("a^2 + b^2");
	p1 *= p2;
	EXPECT_EQ(p1, polynomial::Polynomial{ "a^4 - b^4" });
}

TEST(Polynomial, can_add_monom_with_poly) {
	polynomial::Polynomial p("2xyz");
	polynomial::Monomial m("xyz^2");
	EXPECT_EQ(p + m, polynomial::Polynomial{ "xyz^2 + 2xyz" });
}

TEST(Polynomial, can_sub_monom_with_poly) {
	polynomial::Polynomial p("2xyz");
	polynomial::Monomial m("xyz^2");
	EXPECT_EQ(p - m, polynomial::Polynomial{ "-xyz^2 + 2xyz" });
}

TEST(Polynomial, can_add_polynomials) {
	polynomial::Polynomial p1("2xyzw - 4t^2yz");
	polynomial::Polynomial p2("5t^2yz - xyzw");
	EXPECT_EQ(p1 + p2, polynomial::Polynomial{ "t^2yz + xyzw" });
}

TEST(Polynomial, can_sub_polynomials) {
	polynomial::Polynomial p1("2xyzw - 4t^2yz");
	polynomial::Polynomial p2("5t^2yz - xyzw");
	EXPECT_EQ(p1 - p2, polynomial::Polynomial{ "-9t^2yz + 3xyzw" });
}

TEST(Polynomial, can_mul_poly_by_scalar) {
	polynomial::Polynomial p("2xyzq^2 - 5z^2y^3");
	EXPECT_EQ(p * 5, polynomial::Polynomial{ "10xyzq^2 - 25z^2y^3" });
}

TEST(Polynomial, can_mul_poly_by_monom) {
	polynomial::Polynomial p("abc^3 - ab^2c");
	polynomial::Monomial m("ab");
	EXPECT_EQ(p * m, polynomial::Polynomial{ "a^2b^2c^3 - a^2b^3c" });
}

TEST(Polynomial, can_mul_poly_by_poly) {
	polynomial::Polynomial p1("a - b");
	polynomial::Polynomial p2("a^2 + ab + b^2");
	EXPECT_EQ(p1 * p2, polynomial::Polynomial{ "a^3 - b^3" });
}

TEST(Polynomial, mul_poly_by_scalar_is_commutative) {
	polynomial::Polynomial p("a + y");
	EXPECT_EQ(p * 2.0, 2.0 * p);
}

TEST(Polynomial, mul_poly_by_monom_is_commutative) {
	polynomial::Polynomial p("a + x + y");
	polynomial::Monomial m("a");
	EXPECT_EQ(p * m, m * p);
}

TEST(Polynomial, mul_poly_by_poly_is_commutative) {
	polynomial::Polynomial p1("a + b - 5");
	polynomial::Polynomial p2("a^2 - ab + b^2");
	EXPECT_EQ(p1 * p2, p2 * p1);
}

TEST(Polynomial, mul_by_zero_clears_the_poly) {
	polynomial::Polynomial p("a + b + c + 1");
	p *= 0.0;
	EXPECT_TRUE(p.is_zero());
}

