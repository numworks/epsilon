#include "comparison_operator_popup_data_source.h"

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

void ComparisonOperatorPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell,
                                                                int index) {
  TextHighlightView * bufferCell = static_cast<TextHighlightView *>(cell);

  constexpr int firstParamBufferSize = Constants::k_shortBufferSize;
  char firstParamBuffer[firstParamBufferSize];
  defaultConvertFloatToText(m_hypothesisParams->firstParam(), firstParamBuffer, firstParamBufferSize);
  const char * symbol = testToTextSymbol(App::app()->test());
  constexpr int bufferSize = k_cellBufferSize;
  char buffer[bufferSize];
  snprintf(buffer,
           bufferSize,
           "%s%s%s",
           symbol,
           HypothesisParams::strForComparisonOp(
               static_cast<HypothesisParams::ComparisonOperator>(index)),
           firstParamBuffer);
  bufferCell->setText(buffer);
}

}  // namespace Probability
