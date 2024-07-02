#include <apps/shared/global_context.h>
#include <poincare/old/addition.h>
#include <poincare/old/constant.h>
#include <poincare/old/decimal.h>
#include <poincare/old/old_expression.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/unit.h>
#include <quiz.h>

#include "helper.h"
#include "tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_expression_can_start_uninitialized) {
  OExpression e;
  {
    Rational i = Rational::Builder(1);
    e = i;
  }
}

QUIZ_CASE(poincare_expression_can_be_copied_even_if_uninitialized) {
  OExpression e;
  OExpression f;
  f = e;
}

QUIZ_CASE(poincare_expression_cast_does_not_copy) {
  Rational i1 = Rational::Builder(1);
  Rational i2 = Rational::Builder(2);
  Addition j = Addition::Builder(i1, i2);
  OExpression k = j;
  quiz_assert(k.identifier() == (static_cast<Addition&>(k)).identifier());
  quiz_assert(i1.identifier() == (static_cast<OExpression&>(i1)).identifier());
  quiz_assert(k.identifier() == (static_cast<OExpression&>(k)).identifier());
}

static inline void assert_equal(const Decimal i, const Decimal j) {
  quiz_assert(i.isIdenticalTo(j));
}

static inline void assert_not_equal(const Decimal i, const Decimal j) {
  quiz_assert(!i.isIdenticalTo(j));
}

QUIZ_CASE(poincare_expression_decimal_constructor) {
  int initialPoolSize = pool_size();
  Decimal a = Decimal::Builder("123", 2);
  Decimal b = Decimal::Builder("3456", -4);
  Decimal c = Decimal::Builder(2.34f);
  Decimal d = Decimal::Builder(2322.34);
  assert_pool_size(initialPoolSize + 4);

  assert_equal(Decimal::Builder("25", 3), Decimal::Builder("25", 3));
  assert_equal(Decimal::Builder("25", 3), Decimal::Builder(25, 3));
  assert_not_equal(Decimal::Builder("1000", -3), Decimal::Builder("1", -3));
  assert_not_equal(Decimal::Builder("1000", 3), Decimal::Builder("1", 3));
  assert_not_equal(Decimal::Builder(123, 234), Decimal::Builder(42, 108));
  assert_not_equal(Decimal::Builder(12, 2), Decimal::Builder(123, 2));
  assert_not_equal(Decimal::Builder(1234, 2), Decimal::Builder(1234, 3));
  assert_not_equal(Decimal::Builder(12345, 2), Decimal::Builder(1235, 2));
  assert_not_equal(Decimal::Builder(123456, -2), Decimal::Builder(1234567, -3));
  assert_not_equal(Decimal::Builder(12345678, -2),
                   Decimal::Builder(1234567, -2));
}

QUIZ_CASE(poincare_expression_rational_constructor) {
  int initialPoolSize = pool_size();
  Rational a = Rational::Builder("123", "324");
  Rational b = Rational::Builder("3456");
  Rational c = Rational::Builder(123, 324);
  Rational d = Rational::Builder(3456789);
  Integer overflow = Integer::Overflow(false);
  Rational e = Rational::Builder(overflow);
  Rational f = Rational::Builder(overflow, overflow);
  assert_pool_size(initialPoolSize + 6);
}

QUIZ_CASE(poincare_expression_constant_constructor) {
  Constant c = Constant::PiBuilder();
  c = Constant::Builder("_ke");
}

QUIZ_CASE(poincare_expression_unit_constructor) {
  OUnit u = OUnit::Builder(OUnit::k_timeRepresentatives,
                           OUnit::Prefix::EmptyPrefix());
  assert_expression_serializes_to(u, "_s");

  u = OUnit::Builder(OUnit::k_timeRepresentatives + 2,
                     OUnit::Prefix::EmptyPrefix());
  assert_expression_serializes_to(u, "_h");

  u = OUnit::Builder(OUnit::k_distanceRepresentatives, OUnit::k_prefixes + 9);
  assert_expression_serializes_to(u, "_km");

  u = OUnit::Builder(OUnit::k_volumeRepresentatives,
                     OUnit::Prefix::EmptyPrefix());
  assert_expression_serializes_to(u, "_L");

  u = OUnit::Builder(OUnit::k_powerRepresentatives,
                     OUnit::Prefix::EmptyPrefix());
  assert_expression_serializes_to(u, "_W");
}

static inline void assert_number_of_numerical_values(const char* expression,
                                                     int result) {
  Shared::GlobalContext globalContext;
  quiz_assert(
      parse_expression(expression, &globalContext).numberOfNumericalValues() ==
      result);
}

QUIZ_CASE(poincare_number_of_numerical_values) {
  assert_number_of_numerical_values("2+(3-1)", 3);
  assert_number_of_numerical_values("e", 0);
  assert_number_of_numerical_values("π", 1);
  assert_number_of_numerical_values("√(1+random())", -1);
  assert_number_of_numerical_values("ln(1+inf)", -1);
}

static inline void assert_generalizes_to_and_extract(const char* expression,
                                                     const char* generalized,
                                                     float value) {
  Shared::GlobalContext globalContext;
  OExpression e = parse_expression(expression, &globalContext);
  OExpression g = parse_expression(generalized, &globalContext);
  float v = e.getNumericalValue();
  e.replaceNumericalValuesWithSymbol(Symbol::Builder('x'));
  quiz_assert(e.isIdenticalTo(g));
  quiz_assert(value == v);
}

QUIZ_CASE(poincare_expression_generalization) {
  assert_generalizes_to_and_extract("ln(2)", "ln(x)", 2.f);
  assert_generalizes_to_and_extract("2^3", "x^3", 2.f);
  assert_generalizes_to_and_extract("e^3", "e^x", 3.f);
  assert_generalizes_to_and_extract("√(e+tan(e^(e^(π^4))))",
                                    "√(e+tan(e^(e^(x^4))))", M_PI);
  assert_generalizes_to_and_extract("abs(i+3)", "abs(i+x)", 3.f);
}
