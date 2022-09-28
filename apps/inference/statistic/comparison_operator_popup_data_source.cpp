#include "comparison_operator_popup_data_source.h"
#include <poincare/print.h>
#include "inference/app.h"
#include "inference/text_helpers.h"

namespace Inference {

void ComparisonOperatorPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  BufferTextHighlightCell * bufferCell = static_cast<BufferTextHighlightCell *>(cell);
  const char * symbol = m_test->hypothesisSymbol();
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%s%*.*ed",
      symbol,
      HypothesisParams::strForComparisonOp(static_cast<HypothesisParams::ComparisonOperator>(index)),
      m_test->hypothesisParams()->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  bufferCell->setText(buffer);
}

}  // namespace Inference
