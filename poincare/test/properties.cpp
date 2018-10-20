#include <quiz.h>
#include <poincare/global_context.h>
#include <poincare/expression.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

constexpr Poincare::ExpressionNode::Sign Positive = Poincare::ExpressionNode::Sign::Positive;
constexpr Poincare::ExpressionNode::Sign Negative = Poincare::ExpressionNode::Sign::Negative;
constexpr Poincare::ExpressionNode::Sign Unknown = Poincare::ExpressionNode::Sign::Unknown;

void assert_parsed_expression_sign(const char * expression, Poincare::ExpressionNode::Sign sign) {
  GlobalContext globalContext;
  Expression e = parse_expression(expression);
  quiz_assert(!e.isUninitialized());
  e = e.simplify(globalContext, Degree);
  quiz_assert(e.sign() == sign);
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
  assert_parsed_expression_polynomial_degree("diff(3*x+x,2)", -1);
  assert_parsed_expression_polynomial_degree("diff(3*x+x,x)", -1);
  assert_parsed_expression_polynomial_degree("(3*x+2)/3", 1);
  assert_parsed_expression_polynomial_degree("(3*x+2)/x", -1);
  assert_parsed_expression_polynomial_degree("int(2*x, 0, 1)", -1);
  assert_parsed_expression_polynomial_degree("[[1,2][3,4]]", -1);
  assert_parsed_expression_polynomial_degree("(x^2+2)*(x+1)", 3);
  assert_parsed_expression_polynomial_degree("-(x+1)", 1);
  assert_parsed_expression_polynomial_degree("(x^2+2)^(3)", 6);
  assert_parsed_expression_polynomial_degree("prediction(0.2,10)+1", -1);
  assert_parsed_expression_polynomial_degree("2-x-x^3", 3);
  assert_parsed_expression_polynomial_degree("P*x", 1);
}

void assert_parsed_expression_has_characteristic_range(const char * expression, float range, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) {
  GlobalContext globalContext;
  Expression e = parse_expression(expression);
  quiz_assert(!e.isUninitialized());
  e = e.simplify(globalContext, angleUnit);
  if (std::isnan(range)) {
    quiz_assert(std::isnan(e.characteristicXRange(globalContext, angleUnit)));
  } else {
    quiz_assert(std::fabs(e.characteristicXRange(globalContext, angleUnit) - range) < 0.0000001f);
  }
}

QUIZ_CASE(poincare_characteristic_range) {
  assert_parsed_expression_has_characteristic_range("cos(x)", 360.0f);
  assert_parsed_expression_has_characteristic_range("cos(-x)", 360.0f);
  assert_parsed_expression_has_characteristic_range("cos(x)", 2.0f*M_PI, Preferences::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("cos(-x)", 2.0f*M_PI, Preferences::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)", 40.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)+cos(x/2)", 720.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)+cos(x/2)", 4.0f*M_PI, Preferences::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("x", NAN);
  assert_parsed_expression_has_characteristic_range("cos(3)+2", 0.0f);
  assert_parsed_expression_has_characteristic_range("log(cos(40*x))", 9.0f);
  assert_parsed_expression_has_characteristic_range("cos(cos(x))", 360.0f);
}

void assert_parsed_expression_has_variables(const char * expression, const char * variables) {
  Expression e = parse_expression(expression);
  quiz_assert(!e.isUninitialized());
  char variableBuffer[Expression::k_maxNumberOfVariables+1] = {0};
  int numberOfVariables = e.getVariables(Poincare::Symbol::isVariableSymbol, variableBuffer);
  if (variables == nullptr) {
    quiz_assert(numberOfVariables == -1);
  } else {
    quiz_assert(numberOfVariables == strlen(variables));
    char * currentChar = variableBuffer;
    while (*variables != 0) {
      quiz_assert(*currentChar++ == *variables++);
    }
  }
}

QUIZ_CASE(poincare_get_variables) {
  assert_parsed_expression_has_variables("x+y", "xy");
  assert_parsed_expression_has_variables("x+y+z+2*t", "xyzt");
  assert_parsed_expression_has_variables("abcdef", "abcdef");
  assert_parsed_expression_has_variables("abcdefg", nullptr);
  assert_parsed_expression_has_variables("abcde", "abcde");
  assert_parsed_expression_has_variables("x^2+2*y+k!*A+w", "xykw");
}

void assert_parsed_expression_has_polynomial_coefficient(const char * expression, char symbolName, const char ** coefficients, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) {
  GlobalContext globalContext;
  Expression e = parse_expression(expression);
  quiz_assert(!e.isUninitialized());
  e = e.deepReduce(globalContext, angleUnit);
  Expression coefficientBuffer[Poincare::Expression::k_maxNumberOfPolynomialCoefficients];
  int d = e.getPolynomialReducedCoefficients(symbolName, coefficientBuffer, globalContext, Radian);
  for (int i = 0; i <= d; i++) {
    Expression f = parse_expression(coefficients[i]);
    quiz_assert(!f.isUninitialized());
    coefficientBuffer[i] = coefficientBuffer[i].deepReduce(globalContext, angleUnit);
    f = f.deepReduce(globalContext, angleUnit);
    quiz_assert(coefficientBuffer[i].isIdenticalTo(f));
  }
  quiz_assert(coefficients[d+1] == 0);
}

QUIZ_CASE(poincare_get_polynomial_coefficients) {
  const char * coefficient0[] = {"2", "1", "1", 0};
  assert_parsed_expression_has_polynomial_coefficient("x^2+x+2", 'x', coefficient0);
  const char * coefficient1[] = {"12+(-6)*P", "12", "3", 0}; //3*x^2+12*x-6*π+12
  assert_parsed_expression_has_polynomial_coefficient("3*(x+2)^2-6*P", 'x', coefficient1);
  // TODO: decomment when enable 3-degree polynomes
  //const char * coefficient2[] = {"2+32*x", "2", "6", "2", 0}; //2*n^3+6*n^2+2*n+2+32*x
  //assert_parsed_expression_has_polynomial_coefficient("2*(n+1)^3-4n+32*x", 'n', coefficient2);
  const char * coefficient3[] = {"1", "-P", "1", 0}; //x^2-Pi*x+1
  assert_parsed_expression_has_polynomial_coefficient("x^2-P*x+1", 'x', coefficient3);
}
