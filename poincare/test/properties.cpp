#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/expression.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

constexpr Poincare::ExpressionNode::Sign Positive = Poincare::ExpressionNode::Sign::Positive;
constexpr Poincare::ExpressionNode::Sign Negative = Poincare::ExpressionNode::Sign::Negative;
constexpr Poincare::ExpressionNode::Sign Unknown = Poincare::ExpressionNode::Sign::Unknown;

void assert_parsed_expression_sign(const char * expression, Poincare::ExpressionNode::Sign sign) {
  Shared::GlobalContext globalContext;
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
  assert_parsed_expression_polynomial_degree("diff(3*x+x,x,2)", -1);
  assert_parsed_expression_polynomial_degree("diff(3*x+x,x,x)", -1);
  assert_parsed_expression_polynomial_degree("diff(3*x+x,x,x)", 0, "a");
  assert_parsed_expression_polynomial_degree("(3*x+2)/3", 1);
  assert_parsed_expression_polynomial_degree("(3*x+2)/x", -1);
  assert_parsed_expression_polynomial_degree("int(2*x, 0, 1)", -1);
  assert_parsed_expression_polynomial_degree("int(2*x, 0, 1)", 0, "a");
  assert_parsed_expression_polynomial_degree("[[1,2][3,4]]", -1);
  assert_parsed_expression_polynomial_degree("(x^2+2)*(x+1)", 3);
  assert_parsed_expression_polynomial_degree("-(x+1)", 1);
  assert_parsed_expression_polynomial_degree("(x^2+2)^(3)", 6);
  assert_parsed_expression_polynomial_degree("prediction(0.2,10)+1", -1);
  assert_parsed_expression_polynomial_degree("2-x-x^3", 3);
  assert_parsed_expression_polynomial_degree("P*x", 1);
}

void assert_parsed_expression_has_characteristic_range(const char * expression, float range, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) {
  Shared::GlobalContext globalContext;
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
  assert_parsed_expression_has_characteristic_range("cos(?)", 360.0f);
  assert_parsed_expression_has_characteristic_range("cos(-?)", 360.0f);
  assert_parsed_expression_has_characteristic_range("cos(?)", 2.0f*M_PI, Preferences::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("cos(-?)", 2.0f*M_PI, Preferences::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("sin(9*?+10)", 40.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*?+10)+cos(?/2)", 720.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*?+10)+cos(?/2)", 4.0f*M_PI, Preferences::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("?", NAN);
  assert_parsed_expression_has_characteristic_range("cos(3)+2", 0.0f);
  assert_parsed_expression_has_characteristic_range("log(cos(40*?))", 9.0f);
  assert_parsed_expression_has_characteristic_range("cos(cos(?))", 360.0f);
}

void assert_parsed_expression_has_variables(const char * expression, const char * variables[], int trueNumberOfVariables) {
  Expression e = parse_expression(expression);
  quiz_assert(!e.isUninitialized());
  constexpr static int k_maxVariableSize = 10;
  char variableBuffer[Expression::k_maxNumberOfVariables+1][k_maxVariableSize] = {{0}};
  Shared::GlobalContext globalContext;
  int numberOfVariables = e.getVariables(globalContext, [](const char * symbol) { return true; }, (char *)variableBuffer, k_maxVariableSize);
  quiz_assert(trueNumberOfVariables == numberOfVariables);
  if (numberOfVariables < 0) {
    // Too many variables or variable name too long
    return;
  }
  int index = 0;
  while (variableBuffer[index][0] != 0 || variables[index][0] != 0) {
    quiz_assert(strcmp(variableBuffer[index], variables[index]) == 0);
    index++;
  }
}

QUIZ_CASE(poincare_get_variables) {
  const char * variableBuffer1[] = {"x","y",""};
  assert_parsed_expression_has_variables("x+y", variableBuffer1, 2);
  const char * variableBuffer2[] = {"x","y","z","t",""};
  assert_parsed_expression_has_variables("x+y+z+2*t", variableBuffer2, 4);
  const char * variableBuffer3[] = {"a","x","y","k","A", ""};
  assert_parsed_expression_has_variables("a+x^2+2*y+k!*A", variableBuffer3, 5);
  const char * variableBuffer4[] = {"BABA","abab", ""};
  assert_parsed_expression_has_variables("BABA+abab", variableBuffer4, 2);
  const char * variableBuffer5[] = {""};
  assert_parsed_expression_has_variables("BBBBBBBBBBBBBBBBBBBBBBBBBBBBB", variableBuffer5, -2);
  const char * variableBuffer6[] = {""};
  assert_parsed_expression_has_variables("a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+aa+bb+cc+dd+ee+ff+gg+hh+ii+jj+kk+ll+mm+nn+oo", variableBuffer6, -1);
}

void assert_parsed_expression_has_polynomial_coefficient(const char * expression, const char * symbolName, const char ** coefficients, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Degree) {
  Shared::GlobalContext globalContext;
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
  assert_parsed_expression_has_polynomial_coefficient("x^2+x+2", "x", coefficient0);
  const char * coefficient1[] = {"12+(-6)*P", "12", "3", 0}; //3*x^2+12*x-6*π+12
  assert_parsed_expression_has_polynomial_coefficient("3*(x+2)^2-6*P", "x", coefficient1);
  // TODO: decomment when enable 3-degree polynomes
  //const char * coefficient2[] = {"2+32*x", "2", "6", "2", 0}; //2*n^3+6*n^2+2*n+2+32*x
  //assert_parsed_expression_has_polynomial_coefficient("2*(n+1)^3-4n+32*x", "n", coefficient2);
  const char * coefficient3[] = {"1", "-P", "1", 0}; //x^2-Pi*x+1
  assert_parsed_expression_has_polynomial_coefficient("x^2-P*x+1", "x", coefficient3);
}
