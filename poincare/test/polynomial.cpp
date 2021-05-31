#include "helper.h"
#include <poincare/polynomial.h>
#include <apps/shared/global_context.h>

using namespace Poincare;

template <int N>
void assert_roots_of_polynomial_are(const char * polynomial, const char * const (&roots)[N], const char * delta, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  Shared::GlobalContext context;
  ExpressionNode::ReductionContext reductionContext(&context, complexFormat, angleUnit, Metric, User);

  Expression polynomialExp = parse_expression(polynomial, &context, false).reduce(reductionContext);
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  int degree = polynomialExp.getPolynomialReducedCoefficients(symbol, coefficients, &context, complexFormat, angleUnit, Metric, ReplaceAllDefinedSymbolsWithDefinition);

  Expression deltaExp;
  Expression rootsExp[Expression::k_maxPolynomialDegree];
  int numberOfRoots;

  if (degree == 2) {
    numberOfRoots = Polynomial::QuadraticPolynomialRoots(coefficients[2], coefficients[1], coefficients[0], rootsExp, rootsExp + 1, &deltaExp, &context, complexFormat, angleUnit);
  } else {
    assert(degree == 3);
    numberOfRoots = Polynomial::CubicPolynomialRoots(coefficients[3], coefficients[2], coefficients[1], coefficients[0], rootsExp, rootsExp + 1, rootsExp + 2, &deltaExp, &context, complexFormat, angleUnit);
  }

  int targetNumberOfRoots = (N == 1 && roots[0][0] == '\0') ? 0 : N;
  quiz_assert_print_if_failure(numberOfRoots == targetNumberOfRoots, polynomial);
  assert_expression_serialize_to(deltaExp, delta);
  for (int i = 0; i < targetNumberOfRoots; i++) {
    assert_expression_serialize_to(rootsExp[i], roots[i]);
  }
}

QUIZ_CASE(poincare_polynomial_roots_quadratic) {
  // Real roots
  assert_roots_of_polynomial_are("x^2-3×x+2", {"1", "2"}, "1", Real);
  assert_roots_of_polynomial_are("3×x^2", {"0"}, "0", Real);
  assert_roots_of_polynomial_are("1/3×x^2+2/3×x-5", {"-5", "3"}, "64/9", Real);
  assert_roots_of_polynomial_are("𝐢/5×(x-3)^2", {"3"}, "0", Cartesian);
  assert_roots_of_polynomial_are("(x-2/3)(x+0.2)", {"-1/5", "2/3"}, "169/225", Real);
  assert_roots_of_polynomial_are("√(2)(x-√(3))(x-√(5))", {"√(3)", "√(5)"}, "-4×√(15)+16", Real);

  // ComplexRoots
  assert_roots_of_polynomial_are("x^2+1", {""}, "-4", Real);
  assert_roots_of_polynomial_are("x^2+1", {"-𝐢", "𝐢"}, "-4", Cartesian);
  assert_roots_of_polynomial_are("2𝐢×(x-3𝐢)^2", {"3×𝐢"}, "0", Cartesian);
}

QUIZ_CASE(poincare_polynomial_roots_cubic) {
  assert_roots_of_polynomial_are("x^3-3×x^2+3×x-1", {"1"}, "0", Real);
  assert_roots_of_polynomial_are("1/9×(x+√(2))^3", {"-√(2)"}, "0", Real);
  assert_roots_of_polynomial_are("x^3+x^2-15/4×x-9/2", {"2", "-3/2"}, "0", Real);
  assert_roots_of_polynomial_are("4×x^3+3×x+𝐢", {"-𝐢/2", "𝐢"}, "0", Cartesian);
  assert_roots_of_polynomial_are("(x-√(3)/2)(x^2-x+6/4)", {"√(3)/2"}, "\u0012180×√(3)-465\u0013/16", Real);
  assert_roots_of_polynomial_are("(x-√(3)/2)(x^2-x+6/4)", {"√(3)/2", "1/2-√(5)/2×𝐢", "1/2+√(5)/2×𝐢"}, "\u0012180×√(3)-465\u0013/16", Cartesian);
  assert_roots_of_polynomial_are("(x-1)(x-2)(x-3)", {"1", "2", "3"}, "4", Real);
  assert_roots_of_polynomial_are("x^3-(2+𝐢)×x^2-2×𝐢×x-2+4×𝐢", {"2+𝐢", "-1-𝐢", "1+𝐢"}, "-96+40×𝐢", Cartesian);
}
