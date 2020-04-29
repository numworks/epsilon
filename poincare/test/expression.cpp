#include <quiz.h>
#include <poincare/addition.h>
#include <poincare/decimal.h>
#include <poincare/expression.h>
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

void assert_seconds_split_to(double totalSeconds, const char * splittedTime, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  Expression time = Unit::BuildTimeSplit(totalSeconds, context, complexFormat, angleUnit);
  constexpr static int bufferSize = 100;
  char buffer[bufferSize];
  time.serialize(buffer, bufferSize, DecimalMode);
  quiz_assert_print_if_failure(strcmp(buffer, splittedTime) == 0, splittedTime);
}

QUIZ_CASE(poincare_expression_unit_constructor) {
  Shared::GlobalContext globalContext;
  ExpressionNode::ReductionContext reductionContext = ExpressionNode::ReductionContext(&globalContext, Cartesian, Degree, User);
  // Time
  Unit s = Unit::Second();
  quiz_assert(s.isSecond());
  quiz_assert(!s.isMeter());

  assert_seconds_split_to(1234567890, "39×_year+1×_month+13×_day+19×_h+1×_min+30×_s", &globalContext, Cartesian, Degree);
  assert_seconds_split_to(-122, "-2×_min-2×_s", &globalContext, Cartesian, Degree);

  // Speed
  Expression kilometerPerHour = Multiplication::Builder(
      Unit::Kilometer(),
      Power::Builder(
        Unit::Hour(),
        Rational::Builder(-1)
        )
      );
  kilometerPerHour = kilometerPerHour.reduce(reductionContext);
  Expression meterPerSecond;
  kilometerPerHour = kilometerPerHour.removeUnit(&meterPerSecond);
  quiz_assert(Unit::IsISSpeed(meterPerSecond));

  // Volume
  Expression liter = Unit::Liter();
  liter = liter.reduce(reductionContext);
  Expression meter3;
  liter = liter.removeUnit(&meter3);
  quiz_assert(Unit::IsISVolume(meter3));

  // Energy
  Expression wattHour = Multiplication::Builder(
      Unit::Watt(),
      Unit::Hour()
      );
  wattHour = wattHour.reduce(reductionContext);
  Expression kilogramMeter2PerSecond2;
  wattHour = wattHour.removeUnit(&kilogramMeter2PerSecond2);
  quiz_assert(Unit::IsISEnergy(kilogramMeter2PerSecond2));
}
