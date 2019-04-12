#include <quiz.h>
#include <poincare/decimal.h>
#include <assert.h>
#include "helper.h"
#include "tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_decimal_constructor) {
  int initialPoolSize = pool_size();
  Decimal a = Decimal::Builder("123",2);
  Decimal b = Decimal::Builder("3456", -4);
  Decimal c = Decimal::Builder(2.34f);
  Decimal d = Decimal::Builder(2322.34);
  assert_pool_size(initialPoolSize+4);
}

static inline void assert_equal(const Decimal i, const Decimal j) {
  quiz_assert(i.isIdenticalTo(j));
}

static inline void assert_not_equal(const Decimal i, const Decimal j) {
  quiz_assert(!i.isIdenticalTo(j));
}

QUIZ_CASE(poincare_decimal_compare) {
  assert_equal(Decimal::Builder("25", 3), Decimal::Builder("25", 3));
  assert_equal(Decimal::Builder("1000", -3), Decimal::Builder("1", -3));
  assert_equal(Decimal::Builder("1000", 3), Decimal::Builder("1", 3));
  assert_not_equal(Decimal::Builder(123,234), Decimal::Builder(42, 108));
  assert_not_equal(Decimal::Builder(12,2), Decimal::Builder(123, 2));
  assert_not_equal(Decimal::Builder(1234,2), Decimal::Builder(1234,3));
  assert_not_equal(Decimal::Builder(12345,2), Decimal::Builder(1235,2));
  assert_not_equal(Decimal::Builder(123456, -2),Decimal::Builder(1234567, -3));
  assert_not_equal(Decimal::Builder(12345678, -2),Decimal::Builder(1234567, -2));
}

QUIZ_CASE(poincare_decimal_properties) {
  quiz_assert(Decimal::Builder(-2, 3).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Decimal::Builder(-2, -3).sign() == ExpressionNode::Sign::Negative);
  quiz_assert(Decimal::Builder(2, -3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Decimal::Builder(2, 3).sign() == ExpressionNode::Sign::Positive);
  quiz_assert(Decimal::Builder(0, 1).sign() == ExpressionNode::Sign::Positive);
}

// Simplify

QUIZ_CASE(poincare_decimal_simplify) {
  assert_parsed_expression_simplify_to("-2.3", "-23/10");
  assert_parsed_expression_simplify_to("-232.2ᴇ-4", "-1161/50000");
  assert_parsed_expression_simplify_to("0000.000000ᴇ-2", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
}

QUIZ_CASE(poincare_decimal_approximate) {
  assert_parsed_expression_evaluates_to<float>("1.2343ᴇ-2", "0.012343");
  assert_parsed_expression_evaluates_to<double>("-567.2ᴇ2", "-56720");
}
