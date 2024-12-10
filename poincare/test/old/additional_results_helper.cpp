#include <poincare/additional_results_helper.h>
#include <poincare/expression.h>
#include <quiz.h>

#include "../helper.h"

using namespace Poincare;

static inline void assert_single_numerical_values(const char* expression,
                                                  bool result) {
  UserExpression input = UserExpression::Builder(parse(expression));
  quiz_assert(AdditionalResultsHelper::HasSingleNumericalValue(input) ==
              result);
}

QUIZ_CASE(poincare_additional_results_numerical_values) {
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

QUIZ_CASE(poincare_additional_results_generalization) {
  assert_generalizes_to_and_extract("ln(2)", "ln(x)", 2.f);
  assert_generalizes_to_and_extract("2^3", "x^3", 2.f);
  assert_generalizes_to_and_extract("e^3", "e^x", 3.f);
  assert_generalizes_to_and_extract("√(e+tan(e^(e^(π^4))))",
                                    "√(e+tan(e^(e^(x^4))))", M_PI);
  assert_generalizes_to_and_extract("abs(i+3)", "abs(i+x)", 3.f);
}
