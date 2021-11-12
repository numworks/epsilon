#include "helper.h"
#include <poincare/polynomial.h>
#include <apps/shared/global_context.h>

using namespace Poincare;

template <int N>
void assert_roots_of_polynomial_are(const char * polynomial, const char * const (&roots)[N], const char * delta, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Radian, const char * symbol = "x") {
  Shared::GlobalContext context;
  ExpressionNode::ReductionContext reductionContext(&context, complexFormat, angleUnit, MetricUnitFormat, User);

  Expression polynomialExp = parse_expression(polynomial, &context, false).cloneAndReduce(reductionContext);
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  int degree = polynomialExp.getPolynomialReducedCoefficients(symbol, coefficients, &context, complexFormat, angleUnit, MetricUnitFormat, ReplaceAllDefinedSymbolsWithDefinition);

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
  assert_roots_of_polynomial_are("x^2+x+1-𝐢^(2/30)", {"-√(4×cos(π/30)+4×𝐢×sin(π/30)-3)/2-1/2", "√(4×cos(π/30)+4×𝐢×sin(π/30)-3)/2-1/2"}, "4×cos(π/30)-3+4×sin(π/30)×𝐢", Cartesian);
}

QUIZ_CASE(poincare_polynomial_roots_cubic) {
  assert_roots_of_polynomial_are("x^3-3×x^2+3×x-1", {"1"}, "0", Real);
  assert_roots_of_polynomial_are("1/9×(x+√(2))^3", {"-√(2)"}, "0", Real);
  assert_roots_of_polynomial_are("x^3+x^2-15/4×x-9/2", {"-3/2", "2"}, "0", Real);
  assert_roots_of_polynomial_are("4×x^3+3×x+𝐢", {"-𝐢/2", "𝐢"}, "0", Cartesian);
  assert_roots_of_polynomial_are("x^3-8", {"2", "-1-√(3)×𝐢", "-1+√(3)×𝐢"}, "-1728", Cartesian);
  assert_roots_of_polynomial_are("x^3-8𝐢", {"-2×𝐢", "-√(3)+𝐢", "√(3)+𝐢"}, "1728", Cartesian);
  assert_roots_of_polynomial_are("x^3-13-𝐢", {"-1.124282-2.067764×𝐢", "2.352877+6.022476ᴇ-2×𝐢", "-1.228595+2.007539×𝐢"}, "-4536-702×𝐢", Cartesian);
  assert_roots_of_polynomial_are("x^3-ℯ^(2𝐢π/7)", {"1×ℯ^-1.795196×𝐢", "ℯ^2.991993ᴇ-1×𝐢", "ℯ^2.393594×𝐢"}, "2.7ᴇ1×ℯ^-1.346397×𝐢", Polar);
  assert_roots_of_polynomial_are("x^3-ℯ^(2𝐢π/7)-1", {"1.216877×ℯ^-1.944795×𝐢", "1.216877×ℯ^1.495997ᴇ-1×𝐢", "1.216877×ℯ^2.243995×𝐢"}, "8.766845ᴇ1×ℯ^-2.243995×𝐢", Polar);
  assert_roots_of_polynomial_are("(x-√(3)/2)(x^2-x+6/4)", {"√(3)/2"}, "\u0012180×√(3)-465\u0013/16", Real);
  assert_roots_of_polynomial_are("(x-√(3)/2)(x^2-x+6/4)", {"√(3)/2", "1/2-√(5)/2×𝐢", "1/2+√(5)/2×𝐢"}, "\u0012180×√(3)-465\u0013/16", Cartesian);
  assert_roots_of_polynomial_are("(x-1)(x-2)(x-3)", {"1", "2", "3"}, "4", Real);
  assert_roots_of_polynomial_are("x^3-(2+𝐢)×x^2-2×𝐢×x-2+4×𝐢", {"-1-𝐢", "1+𝐢", "2+𝐢"}, "-96+40×𝐢", Cartesian);
  assert_roots_of_polynomial_are("x^3+3×x^2+3×x+0.7", {"-3.30567ᴇ-1", "-1.334716-5.797459ᴇ-1×𝐢", "-1.334716+5.797459ᴇ-1×𝐢"}, "-243/100", Cartesian);
  assert_roots_of_polynomial_are("x^3+3×x^2+3×x+0.7", {"-3.30567ᴇ-1"}, "-243/100", Real);
  assert_roots_of_polynomial_are("(x-4.231)^3", {"4231/1000"}, "0", Real);
  assert_roots_of_polynomial_are("(x-7/3)(x-π)(x-log(3))", {"log(3)", "7/3", "π"}, "1.598007ᴇ1", Real);
  assert_roots_of_polynomial_are("(x-2𝐢+1)(x+3𝐢-1)(x-𝐢+2)", {"1-3×𝐢", "-2+𝐢", "-1+2×𝐢"}, "-1288-666×𝐢", Cartesian);
  assert_roots_of_polynomial_are("x^3+x^2+x-39999999", {"3.416612ᴇ2", "-1.713306ᴇ2-2.961771ᴇ2×𝐢", "-1.713306ᴇ2+2.961771ᴇ2×𝐢"}, "-43199998400000016", Cartesian);
  assert_roots_of_polynomial_are("(x-π)(x^2+x+1-𝐢^(2/30))", {"π", "-√(4×cos(π/30)+4×𝐢×sin(π/30)-3)/2-1/2", "√(4×cos(π/30)+4×𝐢×sin(π/30)-3)/2-1/2"}, "1.668482ᴇ2+6.817647ᴇ1×𝐢", Cartesian);
}
