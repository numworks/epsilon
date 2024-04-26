#include "comparison_operator_popup_data_source.h"

#include <poincare/old/print.h>

#include "inference/app.h"
#include "inference/text_helpers.h"

using namespace Escher;

namespace Inference {

Poincare::ComparisonNode::OperatorType
ComparisonOperatorPopupDataSource::OperatorTypeForRow(int row) {
  assert(row >= 0 && row < k_numberOfOperators);
  switch (row) {
    case 0:
      return Poincare::ComparisonNode::OperatorType::Inferior;
    case 1:
      return Poincare::ComparisonNode::OperatorType::NotEqual;
    default:
      assert(row == 2);
      return Poincare::ComparisonNode::OperatorType::Superior;
  }
}

void ComparisonOperatorPopupDataSource::updateMessages() {
  const char *symbol = m_test->hypothesisSymbol();
  double firstParam = m_test->hypothesisParams()->firstParam();
  constexpr int bufferSize = 7 /* μ1-μ2 */ + 3 /* ≠ */ +
                             Constants::k_shortFloatNumberOfChars /* float */ +
                             1 /* \0 */;
  char buffer[bufferSize];
  for (int row = 0; row < k_numberOfOperators; row++) {
    Poincare::Print::CustomPrintf(
        buffer, bufferSize, "%s%s%*.*ed", symbol,
        Poincare::ComparisonNode::ComparisonOperatorString(
            OperatorTypeForRow(row)),
        firstParam, Poincare::Preferences::PrintFloatMode::Decimal,
        Poincare::Preferences::ShortNumberOfSignificantDigits);
    m_cells[row].setText(buffer);
  }
}

}  // namespace Inference
