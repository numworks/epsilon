#include "comparison_operator_popup_data_source.h"

#include "probability/app.h"
#include "probability/text_helpers.h"

namespace Probability {

void ComparisonOperatorPopupDataSource::willDisplayCellForIndex(Escher::HighlightCell * cell,
                                                                int index) {
  /* TODO factor with HypothesisController */
  TextHighlightView * bufferCell = static_cast<TextHighlightView *>(cell);

  constexpr int firstParamBufferSize = 10;
  char firstParamBuffer[firstParamBufferSize];
  defaultParseFloat(m_hypothesisParams->firstParam(), firstParamBuffer, firstParamBufferSize);
  const char * symbol = testToTextSymbol(App::app()->test());
  constexpr int bufferSize = 20;
  char buffer[bufferSize];
  sprintf(buffer,
          "%s%c%s",
          symbol,
          HypothesisParams::charForComparisonOp(
              static_cast<HypothesisParams::ComparisonOperator>(index)),
          firstParamBuffer);
  bufferCell->setText(buffer);
}

}  // namespace Probability
