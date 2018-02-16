#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

constexpr Poincare::Expression::Sign Positive = Poincare::Expression::Sign::Positive;
constexpr Poincare::Expression::Sign Negative = Poincare::Expression::Sign::Negative;
constexpr Poincare::Expression::Sign Unknown = Poincare::Expression::Sign::Unknown;

void assert_parsed_expression_sign(const char * expression, Poincare::Expression::Sign sign) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
  Expression::Simplify(&e, globalContext);
  assert(e->sign() == sign);
  delete e;
}

QUIZ_CASE(poincare_sign) {
  assert_parsed_expression_sign("abs(-cos(2))", Positive);
  assert_parsed_expression_sign("2.345E-23", Positive);
  assert_parsed_expression_sign("-2.345E-23", Negative);
  assert_parsed_expression_sign("2*(-3)*abs(-32)", Negative);
  assert_parsed_expression_sign("2*(-3)*abs(-32)*cos(3)", Unknown);
  assert_parsed_expression_sign("2^(-abs(3))", Positive);
  assert_parsed_expression_sign("(-2)^4", Positive);
  assert_parsed_expression_sign("(-2)^3", Negative);
  assert_parsed_expression_sign("random()", Positive);
  assert_parsed_expression_sign("42/3", Positive);
  assert_parsed_expression_sign("-23/32", Negative);
  assert_parsed_expression_sign("P", Positive);
  assert_parsed_expression_sign("X", Positive);
}

QUIZ_CASE(poincare_polynomial_degree) {
  assert_parsed_expression_polynomial_degree("x+1", 1);
  assert_parsed_expression_polynomial_degree("cos(2)+1", 0);
  assert_parsed_expression_polynomial_degree("confidence(0.2,10)+1", -1);
  assert_parsed_expression_polynomial_degree("diff(3*x+x,2)", 0);
  assert_parsed_expression_polynomial_degree("diff(3*x+x,x)", -1);
  assert_parsed_expression_polynomial_degree("(3*x+2)/3", 1);
  assert_parsed_expression_polynomial_degree("(3*x+2)/x", -1);
  assert_parsed_expression_polynomial_degree("int(2*x, 0, 1)", 0);
  assert_parsed_expression_polynomial_degree("[[1,2][3,4]]", -1);
  assert_parsed_expression_polynomial_degree("(x^2+2)*(x+1)", 3);
  assert_parsed_expression_polynomial_degree("-(x+1)", 1);
  assert_parsed_expression_polynomial_degree("(x^2+2)^(3/2)", 3);
  assert_parsed_expression_polynomial_degree("prediction(0.2,10)+1", -1);
  assert_parsed_expression_polynomial_degree("2-x-x^3", 3);
  assert_parsed_expression_polynomial_degree("P*x", 1);
}

void assert_parsed_expression_has_characteristic_range(const char * expression, float range, Expression::AngleUnit angleUnit = Expression::AngleUnit::Degree) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
  Expression::Simplify(&e, globalContext, angleUnit);
  if (std::isnan(range)) {
    assert(std::isnan(e->characteristicXRange(globalContext, angleUnit)));
  } else {
    assert(std::fabs(e->characteristicXRange(globalContext, angleUnit) - range) < 0.0000001f);
  }
  delete e;
}

QUIZ_CASE(poincare_characteristic_range) {
  assert_parsed_expression_has_characteristic_range("cos(x)", 360.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)", 40.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)+cos(x/2)", 720.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)+cos(x/2)", 4.0f*M_PI, Expression::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("x", NAN);
  assert_parsed_expression_has_characteristic_range("cos(3)+2", 0.0f);
  assert_parsed_expression_has_characteristic_range("log(cos(40*x))", 9.0f);
}
