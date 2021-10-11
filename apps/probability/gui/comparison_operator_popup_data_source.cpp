#include "comparison_operator_popup_data_source.h"
#include <poincare/print.h>
#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

void ComparisonOperatorPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  BufferTextHighlightCell * bufferCell = static_cast<BufferTextHighlightCell *>(cell);
  const char * symbol = testToTextSymbol(App::app()->test());
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  Poincare::Print::customPrintf(buffer, bufferSize, "%s%s%*.*ed",
      symbol,
      HypothesisParams::strForComparisonOp(static_cast<HypothesisParams::ComparisonOperator>(index)),
      m_hypothesisParams->firstParam(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  bufferCell->setText(buffer);
}

}  // namespace Probability
