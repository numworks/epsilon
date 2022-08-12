#include <quiz.h>
#include <poincare/addition.h>
#include <poincare/constant.h>
#include <poincare/decimal.h>
#include <poincare/expression.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/unit.h>
#include <apps/shared/global_context.h>
#include "tree/helpers.h"
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_expression_can_start_uninitialized) {
  Expression e;
  {
    Rational i = Rational::Builder(1);
    e = i;
  }
}

QUIZ_CASE(poincare_expression_can_be_copied_even_if_uninitialized) {
  Expression e;
  Expression f;
  f = e;
}

QUIZ_CASE(poincare_expression_cast_does_not_copy) {
  Rational i1 = Rational::Builder(1);
  Rational i2 = Rational::Builder(2);
  Addition j = Addition::Builder(i1, i2);
  Expression k = j;
  quiz_assert(k.identifier() == (static_cast<Addition&>(k)).identifier());
  quiz_assert(i1.identifier() == (static_cast<Expression&>(i1)).identifier());
  quiz_assert(k.identifier() == (static_cast<Expression&>(k)).identifier());
}

static inline void assert_equal(const Decimal i, const Decimal j) {
  quiz_assert(i.isIdenticalTo(j));
}

static inline void assert_not_equal(const Decimal i, const Decimal j) {
  quiz_assert(!i.isIdenticalTo(j));
}

QUIZ_CASE(poincare_expression_decimal_constructor) {
  int initialPoolSize = pool_size();
  Decimal a = Decimal::Builder("123",2);
  Decimal b = Decimal::Builder("3456", -4);
  Decimal c = Decimal::Builder(2.34f);
  Decimal d = Decimal::Builder(2322.34);
  assert_pool_size(initialPoolSize+4);

  assert_equal(Decimal::Builder("25", 3), Decimal::Builder("25", 3));
  assert_equal(Decimal::Builder("25", 3), Decimal::Builder(25, 3));
  assert_not_equal(Decimal::Builder("1000", -3), Decimal::Builder("1", -3));
  assert_not_equal(Decimal::Builder("1000", 3), Decimal::Builder("1", 3));
  assert_not_equal(Decimal::Builder(123,234), Decimal::Builder(42, 108));
  assert_not_equal(Decimal::Builder(12,2), Decimal::Builder(123, 2));
  assert_not_equal(Decimal::Builder(1234,2), Decimal::Builder(1234,3));
  assert_not_equal(Decimal::Builder(12345,2), Decimal::Builder(1235,2));
  assert_not_equal(Decimal::Builder(123456, -2),Decimal::Builder(1234567, -3));
  assert_not_equal(Decimal::Builder(12345678, -2),Decimal::Builder(1234567, -2));
}

QUIZ_CASE(poincare_expression_rational_constructor) {
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

QUIZ_CASE(poincare_expression_constant_constructor) {
  Constant c = Constant::Builder("π");
  c = Constant::Builder("_ke");
}

QUIZ_CASE(poincare_expression_unit_constructor) {
  Unit u = Unit::Builder(Unit::k_timeRepresentatives, Unit::Prefix::EmptyPrefix());
  assert_expression_serialize_to(u, "_s");

  u = Unit::Builder(Unit::k_timeRepresentatives + 2, Unit::Prefix::EmptyPrefix());
  assert_expression_serialize_to(u, "_h");

  u = Unit::Builder(Unit::k_distanceRepresentatives, Unit::k_prefixes + 9);
  assert_expression_serialize_to(u, "_km");

  u = Unit::Builder(Unit::k_volumeRepresentatives, Unit::Prefix::EmptyPrefix());
  assert_expression_serialize_to(u, "_L");

  u = Unit::Builder(Unit::k_powerRepresentatives, Unit::Prefix::EmptyPrefix());
  assert_expression_serialize_to(u, "_W");
}

static inline void assert_generalizes_to(Expression e, const Expression f) {
  quiz_assert(e.replaceNumericalValuesWithSymbol(Symbol::Builder('x')).isIdenticalTo(f));
}

QUIZ_CASE(poincare_expression_generalization) {
  Symbol x = Symbol::Builder('x');
  assert_generalizes_to(Function::Builder("ln", 2, Decimal::Builder("2", 1)),
                        Function::Builder("ln", 2, x));
  assert_generalizes_to(Power::Builder(Decimal::Builder("2", 1), Decimal::Builder("3", 1)),
                        Power::Builder(x, Decimal::Builder("3", 1)));
  assert_generalizes_to(Power::Builder(Constant::Builder("e"), Decimal::Builder("3", 1)),
                        Power::Builder(Constant::Builder("e"), x));
}
