#include <apps/shared/global_context.h>
#include <poincare/polynomial.h>

#include "helper.h"

using namespace Poincare;

template <int N>
void assert_roots_of_polynomial_are(const char* polynomial,
                                    const char* const (&roots)[N],
                                    const char* delta,
                                    Preferences::ComplexFormat complexFormat,
                                    Preferences::AngleUnit angleUnit = Radian,
                                    const char* symbol = "x") {
  Shared::GlobalContext context;
  ReductionContext reductionContext(&context, complexFormat, angleUnit,
                                    MetricUnitFormat, User);

  Expression polynomialExp = parse_expression(polynomial, &context, false)
                                 .cloneAndReduce(reductionContext);
  Expression coefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  int degree = polynomialExp.getPolynomialReducedCoefficients(
      symbol, coefficients, &context, complexFormat, angleUnit,
      MetricUnitFormat, ReplaceAllDefinedSymbolsWithDefinition);

  Expression deltaExp;
  Expression rootsExp[Expression::k_maxPolynomialDegree];
  int numberOfRoots;
  if (degree == 2) {
    numberOfRoots = Polynomial::QuadraticPolynomialRoots(
        coefficients[2], coefficients[1], coefficients[0], rootsExp,
        rootsExp + 1, &deltaExp, reductionContext);
  } else {
    assert(degree == 3);
    numberOfRoots = Polynomial::CubicPolynomialRoots(
        coefficients[3], coefficients[2], coefficients[1], coefficients[0],
        rootsExp, rootsExp + 1, rootsExp + 2, &deltaExp, reductionContext);
  }

  int targetNumberOfRoots = (N == 1 && roots[0][0] == '\0') ? 0 : N;
  quiz_assert_print_if_failure(numberOfRoots == targetNumberOfRoots,
                               polynomial);
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
  assert_roots_of_polynomial_are("i/5×(x-3)^2", {"3"}, "0", Cartesian);
  assert_roots_of_polynomial_are("(x-2/3)(x+0.2)", {"-1/5", "2/3"}, "169/225",
                                 Real);
  assert_roots_of_polynomial_are("√(2)(x-√(3))(x-√(5))", {"√(3)", "√(5)"},
                                 "16-4×√(15)", Real);

  // ComplexRoots
  assert_roots_of_polynomial_are("x^2+1", {""}, "-4", Real);
  assert_roots_of_polynomial_are("x^2+1", {"-i", "i"}, "-4", Cartesian);
  assert_roots_of_polynomial_are("2i×(x-3i)^2", {"3×i"}, "0", Cartesian);
}

QUIZ_CASE(poincare_polynomial_roots_cubic) {
  assert_roots_of_polynomial_are("x^3-3×x^2+3×x-1", {"1"}, "0", Real);
  assert_roots_of_polynomial_are("1/9×(x+√(2))^3", {"-√(2)"}, "0", Real);
  assert_roots_of_polynomial_are("x^3+x^2-15/4×x-9/2", {"-3/2", "2"}, "0",
                                 Real);
  assert_roots_of_polynomial_are("4×x^3+3×x+i", {"-i/2", "i"}, "0", Cartesian);
  assert_roots_of_polynomial_are("x^3-8", {"2", "-1-√(3)×i", "-1+√(3)×i"},
                                 "-1728", Cartesian);
  assert_roots_of_polynomial_are("x^3-8i", {"-√(3)+i", "-2×i", "√(3)+i"},
                                 "1728", Cartesian);
  assert_roots_of_polynomial_are(
      "x^3-13-i",
      {"-1.228595+2.007539×i", "-1.124282-2.067764×i",
       "2.352877+6.022476ᴇ-2×i"},
      "-4536-702×i", Cartesian);
  assert_roots_of_polynomial_are(
      "2x^3-e^(2iπ/7)",
      {"7.937005ᴇ-1×e^2.393594×i", "7.937005ᴇ-1×e^-1.795196×i",
       "7.937005ᴇ-1×e^2.991993ᴇ-1×i"},
      "1.08ᴇ2×e^-1.346397×i", Polar);
  assert_roots_of_polynomial_are(
      "x^3-e^(2iπ/7)-1",
      {"1.216877×e^2.243995×i", "1.216877×e^-1.944795×i",
       "1.216877×e^1.495997ᴇ-1×i"},
      "8.766845ᴇ1×e^-2.243995×i", Polar);
  assert_roots_of_polynomial_are("(x-√(3)/2)(x^2-x+6/4)", {"√(3)/2"},
                                 "\u0012-465+180×√(3)\u0013/16", Real);
  assert_roots_of_polynomial_are("(x-√(3)/2)(x^2-x+6/4)",
                                 {"√(3)/2", "1/2-√(5)/2×i", "1/2+√(5)/2×i"},
                                 "\u0012-465+180×√(3)\u0013/16", Cartesian);
  assert_roots_of_polynomial_are("(x-1)(x-2)(x-3)", {"1", "2", "3"}, "4", Real);
  assert_roots_of_polynomial_are("x^3-(2+i)×x^2-2×i×x-2+4×i",
                                 {"-1-i", "1+i", "2+i"}, "-96+40×i", Cartesian);
  assert_roots_of_polynomial_are(
      "x^3+3×x^2+3×x+0.7",
      {"-3.30567ᴇ-1", "-1.334716-5.797459ᴇ-1×i", "-1.334716+5.797459ᴇ-1×i"},
      "-243/100", Cartesian);
  assert_roots_of_polynomial_are("x^3+3×x^2+3×x+0.7", {"-3.30567ᴇ-1"},
                                 "-243/100", Real);
  assert_roots_of_polynomial_are("(x-4.231)^3", {"4231/1000"}, "0", Real);
  assert_roots_of_polynomial_are("(x-7/3)(x-π)(x-log(3))",
                                 {"log(3)", "7/3", "π"}, "1.598007ᴇ1", Real);
  assert_roots_of_polynomial_are("(x-2i+1)(x+3i-1)(x-i+2)",
                                 {"-2+1×i", "-1+2×i", "1-3×i"}, "-1288-666×i",
                                 Cartesian);
  assert_roots_of_polynomial_are(
      "x^3+x^2+x-39999999",
      {"3.416612ᴇ2", "-1.713306ᴇ2-2.961771ᴇ2×i", "-1.713306ᴇ2+2.961771ᴇ2×i"},
      "-43199998400000016", Cartesian);
  assert_roots_of_polynomial_are(
      "x^3+x^2+x+1-80*π*200000",
      {"3.687201ᴇ2", "-1.8486ᴇ2-3.196107ᴇ2×i", "-1.8486ᴇ2+3.196107ᴇ2×i"},
      "-6912000000000000×π^2+640000000×π-16", Cartesian);
}
