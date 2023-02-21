#include <poincare/rational.h>

#include "helper.h"

using namespace Poincare;

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

QUIZ_CASE(poincare_rational_order) {
  assert_equal(Rational::Builder(123, 324), Rational::Builder(41, 108));
  assert_not_equal(Rational::Builder(123, 234), Rational::Builder(42, 108));
  assert_lower(Rational::Builder(123, 234), Rational::Builder(456, 567));
  assert_lower(Rational::Builder(-123, 234), Rational::Builder(456, 567));
  assert_greater(Rational::Builder(123, 234), Rational::Builder(-456, 567));
  assert_greater(
      Rational::Builder(123, 234),
      Rational::Builder("123456789123456789", "12345678912345678910"));
}

QUIZ_CASE(poincare_rational_specific_properties) {
  quiz_assert(Rational::Builder(0).isZero());
  quiz_assert(!Rational::Builder(231).isZero());
  quiz_assert(Rational::Builder(1).isOne());
  quiz_assert(!Rational::Builder(-1).isOne());
  quiz_assert(!Rational::Builder(1).isMinusOne());
  quiz_assert(Rational::Builder(-1).isMinusOne());
  quiz_assert(Rational::Builder(1, 2).isHalf());
  quiz_assert(!Rational::Builder(-1).isHalf());
  quiz_assert(Rational::Builder(-1, 2).isMinusHalf());
  quiz_assert(!Rational::Builder(3, 2).isMinusHalf());
  quiz_assert(Rational::Builder(10).isTen());
  quiz_assert(!Rational::Builder(-1).isTen());
  quiz_assert(Rational::Builder(-1).isInteger());
  quiz_assert(Rational::Builder(9).isInteger());
  quiz_assert(Rational::Builder(9, 3).isInteger());
  quiz_assert(Rational::Builder(-9, 3).isInteger());
  quiz_assert(!Rational::Builder(9, 10).isInteger());
  Rational m1 = Rational::IntegerPower(Rational::Builder(2), Integer(1024));
  quiz_assert(m1.numeratorOrDenominatorIsInfinity());
  quiz_assert(!m1.integerDenominator().isZero());
}

static inline void assert_add_to(const Rational i, const Rational j,
                                 const Rational k) {
  quiz_assert(Rational::NaturalOrder(Rational::Addition(i, j), k) == 0);
}

QUIZ_CASE(poincare_rational_addition) {
  assert_add_to(Rational::Builder(1, 2), Rational::Builder(1),
                Rational::Builder(3, 2));
  assert_add_to(Rational::Builder("18446744073709551616", "4294967296"),
                Rational::Builder(8, 9), Rational::Builder("38654705672", "9"));
  assert_add_to(Rational::Builder("18446744073709551616", "4294967296"),
                Rational::Builder(-8, 9),
                Rational::Builder("38654705656", "9"));
}

static inline void assert_pow_to(const Rational i, const Integer j,
                                 const Rational k) {
  quiz_assert(Rational::NaturalOrder(Rational::IntegerPower(i, j), k) == 0);
}

QUIZ_CASE(poincare_rational_power) {
  assert_pow_to(Rational::Builder(4, 5),
                Rational::Builder(3).signedIntegerNumerator(),
                Rational::Builder(64, 125));
  assert_pow_to(Rational::Builder(4, 5),
                Rational::Builder(-3).signedIntegerNumerator(),
                Rational::Builder(125, 64));
}
