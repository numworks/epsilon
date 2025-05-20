
#include "helper.h"
#include "poincare/expression.h"
#include "poincare/k_tree.h"
#include "poincare/preferences.h"

void assert_expression_serializes_to(
    Poincare::SystemExpression expression, const char* serialization,
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

QUIZ_CASE(pcj_expression_serialization) {
  assert_expression_serializes_to(
      Poincare::SystemExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)),
      "(2×π)/3", false);
  // TODO: remove parentheses in compact mode
  assert_expression_serializes_to(
      Poincare::SystemExpression::Builder(KDiv(KMult(2_e, π_e), 3_e)), "(2π)/3",
      true);
}
