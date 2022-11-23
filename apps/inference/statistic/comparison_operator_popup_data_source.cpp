#include "comparison_operator_popup_data_source.h"
#include "inference/app.h"
#include "inference/text_helpers.h"
#include <poincare/print.h>

using namespace Escher;

namespace Inference {

Poincare::ComparisonNode::OperatorType ComparisonOperatorPopupDataSource::OperatorTypeForRow(int row) {
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

void ComparisonOperatorPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  BufferTextHighlightCell * bufferCell = static_cast<BufferTextHighlightCell *>(cell);
  const char * symbol = m_test->hypothesisSymbol();
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%s%*.*ed",
      symbol,
      Poincare::ComparisonNode::ComparisonOperatorString(OperatorTypeForRow(index)),
      m_test->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  bufferCell->setText(buffer);
}

}  // namespace Inference
