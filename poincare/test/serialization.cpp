#include <poincare/expression.h>
#include <poincare/expression_or_float.h>
#include <poincare/k_tree.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>

#include "helper.h"

void assert_expression_serializes_to(
    Poincare::UserExpression expression, const char* serialization,
    bool compactMode = false,
    Poincare::Preferences::PrintFloatMode printFloatMode =
        Poincare::Preferences::PrintFloatMode::Decimal,
    int numberOfSignificantDigits =
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits) {
  constexpr int bufferSize = 100;
  char buffer[bufferSize];
  expression.serialize(buffer, bufferSize, compactMode, printFloatMode,
                       numberOfSignificantDigits);
  quiz_assert_print_if_failure((strcmp(serialization, buffer) == 0),
                               serialization, serialization, buffer);
}

void assert_expression_or_float_serializes_to(
    Poincare::ExpressionOrFloat expressionOrFloat, const char* serialization,
    Poincare::Preferences::PrintFloatMode printFloatMode =
        Poincare::Preferences::PrintFloatMode::Decimal,
    int numberOfSignificantDigits =
        Poincare::Preferences::VeryLargeNumberOfSignificantDigits,
    size_t maxGlyphLength = Poincare::PrintFloat::k_maxFloatGlyphLength) {
  constexpr int bufferSize = 100;
  char buffer[bufferSize];
  expressionOrFloat.writeText(buffer, numberOfSignificantDigits, printFloatMode,
                              maxGlyphLength);
  quiz_assert_print_if_failure((strcmp(serialization, buffer) == 0),
                               serialization, serialization, buffer);
}

QUIZ_CASE(pcj_expression_serialization) {
  using namespace Poincare;
  assert_expression_serializes_to(
      UserExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)), "(2×π)/3", false);
  assert_expression_serializes_to(
      UserExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)), "2π/3", true);
  assert_expression_serializes_to(
      UserExpression::Builder(KMult(KDiv(π_e, 2_e), e_e)), "π/2×e", true);
}

QUIZ_CASE(pcj_expression_or_float_serialization) {
  using namespace Poincare;
  assert_expression_or_float_serializes_to(ExpressionOrFloat(0.123f), "0.123");
  assert_expression_or_float_serializes_to(ExpressionOrFloat(-2.5f), "-2.5");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(static_cast<float>(M_PI)), "3.141593");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(static_cast<float>(M_PI)), "3.142",
      Preferences::PrintFloatMode::Decimal,
      Preferences::ShortNumberOfSignificantDigits);

  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(UserExpression::Builder(π_e)), "π");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(UserExpression::Builder(KDiv(KMult(2_e, π_e), 3_e))),
      "2π/3");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(UserExpression::Builder(KDiv(KMult(2_e, π_e), 30_e))),
      "2π/30");
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(
          UserExpression::Builder(KMult(-1_e, KDiv(KMult(2_e, π_e), 30_e)))),
      "-0.2094395");

  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(UserExpression::Builder(KDiv(KMult(10_e, π_e), 3_e))),
      "", Preferences::PrintFloatMode::Decimal,
      Preferences::VeryLargeNumberOfSignificantDigits, 4);
  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(UserExpression::Builder(KDiv(KMult(10_e, π_e), 3_e))),
      "10.5", Preferences::PrintFloatMode::Decimal,
      Preferences::VeryShortNumberOfSignificantDigits, 4);

  assert_expression_or_float_serializes_to(
      ExpressionOrFloat(UserExpression::Builder(KSin(KSin(KSin(KSin(1_e)))))),
      "0.6275718", Preferences::PrintFloatMode::Decimal);
}
