#include <quiz.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <assert.h>
#include "helper.h"
#include "tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_rational_constructor) {
  int initialPoolSize = pool_size();
  Rational a = Rational::Builder("123","324");
  Rational b = Rational::Builder("3456");
  Rational c = Rational::Builder(123,324);
  Rational d = Rational::Builder(3456789);
  Integer overflow = Integer::Overflow(false);
  Rational e = Rational::Builder(overflow);
  Rational f = Rational::Builder(overflow, overflow);
  assert_pool_size(initialPoolSize+6);
}

static inline void assert_equal(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) == 0);
}
static inline void assert_not_equal(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) != 0);
}

static inline void assert_lower(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) < 0);
}

static inline void assert_greater(const Rational i, const Rational j) {
  quiz_assert(Rational::NaturalOrder(i, j) > 0);
}

QUIZ_CASE(poincare_rational_compare) {
  assert_equal(Rational::Builder(123,324), Rational::Builder(41,108));
  assert_not_equal(Rational::Builder(123,234), Rational::Builder(42, 108));
  assert_lower(Rational::Builder(123,234), Rational::Builder(456,567));
  assert_lower(Rational::Builder(-123, 234),Rational::Builder(456, 567));
  assert_greater(Rational::Builder(123, 234),Rational::Builder(-456, 567));
  assert_greater(Rational::Builder(123, 234),Rational::Builder("123456789123456789", "12345678912345678910"));
}

QUIZ_CASE(poincare_rational_properties) {
  quiz_assert(Rational::Builder(-2).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Rational::Builder(-2, 3).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Rational::Builder(2, 3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Rational::Builder(0, 3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Rational::Builder(0).isZero());
  quiz_assert(!Rational::Builder(231).isZero());
  quiz_assert(Rational::Builder(1).isOne());
  quiz_assert(!Rational::Builder(-1).isOne());
  quiz_assert(!Rational::Builder(1).isMinusOne());
  quiz_assert(Rational::Builder(-1).isMinusOne());
  quiz_assert(Rational::Builder(1,2).isHalf());
  quiz_assert(!Rational::Builder(-1).isHalf());
  quiz_assert(Rational::Builder(-1,2).isMinusHalf());
  quiz_assert(!Rational::Builder(3,2).isMinusHalf());
  quiz_assert(Rational::Builder(10).isTen());
  quiz_assert(!Rational::Builder(-1).isTen());
}

static inline void assert_add_to(const Rational i, const Rational j, const Rational k) {
  quiz_assert(Rational::NaturalOrder(Rational::Addition(i, j), k) == 0);
}

QUIZ_CASE(poincare_rational_addition) {
  assert_add_to(Rational::Builder(1,2), Rational::Builder(1), Rational::Builder(3,2));
  assert_add_to(Rational::Builder("18446744073709551616","4294967296"), Rational::Builder(8,9), Rational::Builder("38654705672","9"));
  assert_add_to(Rational::Builder("18446744073709551616","4294967296"), Rational::Builder(-8,9), Rational::Builder("38654705656","9"));
}

static inline void assert_pow_to(const Rational i,const Integer j, const Rational k) {
  quiz_assert(Rational::NaturalOrder(Rational::IntegerPower(i, j), k) == 0);
}

QUIZ_CASE(poincare_rational_power) {
  assert_pow_to(Rational::Builder(4,5), Rational::Builder(3).signedIntegerNumerator(), Rational::Builder(64,125));
  assert_pow_to(Rational::Builder(4,5), Rational::Builder(-3).signedIntegerNumerator(), Rational::Builder(125,64));
}

// Simplify

QUIZ_CASE(poincare_rational_simplify) {
  // 1/MaxIntegerString()
  char buffer[400] = "1/";
  strlcpy(buffer+2, MaxIntegerString(), 400-2);
  assert_parsed_expression_simplify_to(buffer, buffer);
  // 1/OverflowedIntegerString()
  strlcpy(buffer+2, BigOverflowedIntegerString(), 400-2);
  assert_parsed_expression_simplify_to(buffer, "0");
  // MaxIntegerString()
  assert_parsed_expression_simplify_to(MaxIntegerString(), MaxIntegerString());
  // OverflowedIntegerString()
  assert_parsed_expression_simplify_to(BigOverflowedIntegerString(), Infinity::Name());
  assert_parsed_expression_simplify_to(BigOverflowedIntegerString(), Infinity::Name());
  // -OverflowedIntegerString()
  buffer[0] = '-';
  strlcpy(buffer+1, BigOverflowedIntegerString(), 400-1);
  assert_parsed_expression_simplify_to(buffer, "-inf");

  assert_parsed_expression_simplify_to("-1/3", "-1/3");
  assert_parsed_expression_simplify_to("22355/45325", "4471/9065");
  assert_parsed_expression_simplify_to("0000.000000", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
  assert_parsed_expression_simplify_to("0.1234567", "1234567/10000000");
  assert_parsed_expression_simplify_to("123.4567", "1234567/10000");
  assert_parsed_expression_simplify_to("0.1234", "617/5000");
  assert_parsed_expression_simplify_to("0.1234000", "617/5000");
  assert_parsed_expression_simplify_to("001234000", "1234000");
  assert_parsed_expression_simplify_to("001.234000ᴇ3", "1234");
  assert_parsed_expression_simplify_to("001234000ᴇ-4", "617/5");
  assert_parsed_expression_simplify_to("3/4+5/4-12+1/567", "-5669/567");
  assert_parsed_expression_simplify_to("34/78+67^(-1)", "1178/2613");
  assert_parsed_expression_simplify_to("12348/34564", "3087/8641");
  assert_parsed_expression_simplify_to("1-0.3-0.7", "0");
  assert_parsed_expression_simplify_to("123456789123456789+112233445566778899", "235690234690235688");
  assert_parsed_expression_simplify_to("56^56", "79164324866862966607842406018063254671922245312646690223362402918484170424104310169552592050323456");
  assert_parsed_expression_simplify_to("999^999", "999^999");
  assert_parsed_expression_simplify_to("999^-999", "1/999^999");
  assert_parsed_expression_simplify_to("0^0", Undefined::Name());
  assert_parsed_expression_simplify_to("x^0", "1");
  assert_parsed_expression_simplify_to("π^0", "1");
  assert_parsed_expression_simplify_to("A^0", "1");
  assert_parsed_expression_simplify_to("(-3)^0", "1");
}

QUIZ_CASE(poincare_rational_approximate) {
  assert_parsed_expression_evaluates_to<float>("1/3", "0.3333333");
  assert_parsed_expression_evaluates_to<double>("123456/1234567", "9.9999432999586ᴇ-2");
}


//Serialize

QUIZ_CASE(poincare_rational_serialize) {
  assert_parsed_expression_serialize_to(Rational::Builder(-2, 3), "-2/3");
  assert_parsed_expression_serialize_to(Rational::Builder("2345678909876"), "2345678909876");
  assert_parsed_expression_serialize_to(Rational::Builder("-2345678909876", "5"), "-2345678909876/5");
  assert_parsed_expression_serialize_to(Rational::Builder(MaxIntegerString()), MaxIntegerString());
  Integer one(1);
  Integer overflow = Integer::Overflow(false);
  assert_parsed_expression_serialize_to(Rational::Builder(one, overflow), "1/inf");
  assert_parsed_expression_serialize_to(Rational::Builder(overflow), Infinity::Name());
}
