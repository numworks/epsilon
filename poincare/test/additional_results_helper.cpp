#include <poincare/additional_results_helper.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/user_expression.h>

#include "helper.h"

using namespace Poincare;

static inline void assert_single_numerical_values(const char* expression,
                                                  bool result) {
  UserExpression input = UserExpression::Builder(parse(expression));
  quiz_assert(AdditionalResultsHelper::HasSingleNumericalValue(input) ==
              result);
}

QUIZ_CASE(pcj_additional_results_numerical_values) {
  assert_single_numerical_values("2+(3-1)", false);
  assert_single_numerical_values("e", false);
  assert_single_numerical_values("π", true);
  assert_single_numerical_values("√(1+random())", false);
  assert_single_numerical_values("ln(1+inf)", false);
  assert_single_numerical_values("ln(1+x)", false);
  assert_single_numerical_values("2+(i-abs(i))", true);
  assert_single_numerical_values("4.2^1.5", true);
}

static inline void assert_generalizes_to_and_extract(const char* expression,
                                                     const char* generalized,
                                                     float value) {
  UserExpression e = UserExpression::Builder(parse(expression));
  UserExpression g = UserExpression::Builder(parse(generalized));
  float v;
  UserExpression f =
      AdditionalResultsHelper::CloneReplacingNumericalValuesWithSymbol(e, "x",
                                                                       &v);
  quiz_assert(value == v);
  quiz_assert(g.isIdenticalTo(f));
}

QUIZ_CASE(pcj_additional_results_generalization) {
  assert_generalizes_to_and_extract("ln(2)", "ln(x)", 2.f);
  assert_generalizes_to_and_extract("2^3", "x^3", 2.f);
  assert_generalizes_to_and_extract("e^3", "e^x", 3.f);
  assert_generalizes_to_and_extract("√(e+tan(e^(e^(π^4))))",
                                    "√(e+tan(e^(e^(x^4))))", M_PI);
  assert_generalizes_to_and_extract("abs(i+3)", "abs(i+x)", 3.f);
}

static void assert_rational_approximation_is(int numerator, int denominator,
                                             bool negative,
                                             const char* approximate) {
  const Internal::Tree* e = Internal::SharedTreeStack->pushDiv();
  Internal::SharedTreeStack->pushInteger(numerator);
  Internal::SharedTreeStack->pushInteger(denominator);
  UserExpression expr = UserExpression::Builder(e);
  Layout l =
      AdditionalResultsHelper::CreateRationalApproximation(expr, negative);
  if (!approximate) {
    quiz_assert(l.isUninitialized());
  } else {
    Internal::Tree* result = parse(approximate);
    Internal::TreeRef rack = Internal::Layouter::LayoutExpression(result);
    result->removeTree();
    Internal::NAry::AddChildAtIndex(rack, Internal::CodePointLayout::Push('~'),
                                    0);
    Layout expected = Layout::Builder(rack);
    quiz_assert(l.isIdenticalTo(expected));
  }
}

QUIZ_CASE(pcj_additional_results_rational_approximation) {
  assert_rational_approximation_is(1, 100, false, nullptr);
  assert_rational_approximation_is(101, 10000, false, "1/99");
  assert_rational_approximation_is(1, 3, false, nullptr);
  assert_rational_approximation_is(3333, 10000, false, nullptr);
  assert_rational_approximation_is(33333, 100000, false, "1/3");
  assert_rational_approximation_is(4667, 10000, true, nullptr);
  assert_rational_approximation_is(46667, 100000, true, "-7/15");
}
