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
  assert_parsed_expression_polynomial_degree("(x^2+2)^(3)", 6);
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
  assert_parsed_expression_has_characteristic_range("cos(-x)", 360.0f);
  assert_parsed_expression_has_characteristic_range("cos(x)", 2.0f*M_PI, Expression::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("cos(-x)", 2.0f*M_PI, Expression::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)", 40.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)+cos(x/2)", 720.0f);
  assert_parsed_expression_has_characteristic_range("sin(9*x+10)+cos(x/2)", 4.0f*M_PI, Expression::AngleUnit::Radian);
  assert_parsed_expression_has_characteristic_range("x", NAN);
  assert_parsed_expression_has_characteristic_range("cos(3)+2", 0.0f);
  assert_parsed_expression_has_characteristic_range("log(cos(40*x))", 9.0f);
  assert_parsed_expression_has_characteristic_range("cos(cos(x))", 360.0f);
}

void assert_parsed_expression_has_variables(const char * expression, const char * variables) {
  Expression * e = parse_expression(expression);
  char variableBuffer[Expression::k_maxNumberOfVariables+1] = {0};
  int numberOfVariables = e->getVariables(Poincare::Symbol::isVariableSymbol, variableBuffer);
  if (variables == nullptr) {
    assert(numberOfVariables == -1);
  } else {
    assert(numberOfVariables == strlen(variables));
    char * currentChar = variableBuffer;
    while (*variables != 0) {
      assert(*currentChar++ == *variables++);
    }
  }
  delete e;
}

QUIZ_CASE(poincare_get_variables) {
  assert_parsed_expression_has_variables("x+y", "xy");
  assert_parsed_expression_has_variables("x+y+z+2*t", "xyzt");
  assert_parsed_expression_has_variables("abcdef", "abcdef");
  assert_parsed_expression_has_variables("abcdefg", nullptr);
  assert_parsed_expression_has_variables("abcde", "abcde");
  assert_parsed_expression_has_variables("x^2+2*y+k!*A+w", "xykw");
}

void assert_parsed_expression_has_polynomial_coefficient(const char * expression, char symbolName, const char ** coefficients, Expression::AngleUnit angleUnit = Expression::AngleUnit::Degree) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
  Expression::Reduce(&e, globalContext, angleUnit);
  Expression * coefficientBuffer[Poincare::Expression::k_maxNumberOfPolynomialCoefficients];
  int d = e->getPolynomialCoefficients(symbolName, coefficientBuffer, globalContext);
  for (int i = 0; i <= d; i++) {
    Expression * f = parse_expression(coefficients[i]);
    Expression::Reduce(&coefficientBuffer[i], globalContext, angleUnit);
    Expression::Reduce(&f, globalContext, angleUnit);
    assert(coefficientBuffer[i]->isIdenticalTo(f));
    delete f;
    delete coefficientBuffer[i];
  }
  assert(coefficients[d+1] == 0);
  delete e;
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
