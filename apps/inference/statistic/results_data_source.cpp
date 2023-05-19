#include "results_data_source.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout.h>
#include <poincare/layout_helper.h>

#include "inference/app.h"
#include "inference/constants.h"

using namespace Escher;

namespace Inference {

ResultsDataSource::ResultsDataSource(
    Responder *parent, Statistic *statistic, Invocation invocation,
    DynamicCellsDataSourceDelegate<ResultCell> *dynamicCellsDataSourceDelegate)
    : MemoizedListViewDataSource(),
      DynamicCellsDataSource<ResultCell, k_maxNumberOfResultCells>(
          dynamicCellsDataSourceDelegate),
      m_statistic(statistic),
      m_next(parent, I18n::Message::Next, invocation) {}

int ResultsDataSource::numberOfRows() const {
  return m_statistic->numberOfResults() + 1 /* button */;
}

KDCoordinate ResultsDataSource::defaultColumnWidth() {
  return Ion::Display::Width - Metric::CommonLeftMargin -
         Metric::CommonRightMargin;
}

void ResultsDataSource::willDisplayCellForIndex(HighlightCell *cell, int i) {
  if (i < numberOfRows() - 1) {
    ResultCell *messageCell = static_cast<ResultCell *>(cell);
    double value;
    Poincare::Layout message;
    I18n::Message subMessage;
    int precision = Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
    m_statistic->resultAtIndex(i, &value, &message, &subMessage, &precision);
    constexpr int bufferSize = Constants::k_largeBufferSize;
    char buffer[bufferSize];
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode(
        value, buffer, bufferSize, precision,
        Poincare::Preferences::PrintFloatMode::Decimal);

    messageCell->label()->setLayout(message);
    messageCell->subLabel()->setMessage(subMessage);
    messageCell->accessory()->setText(buffer);
  }
}

HighlightCell *ResultsDataSource::reusableCell(int index, int type) {
  if (type == k_resultCellType) {
    return cell(index);
  }
  return &m_next;
}

int ResultsDataSource::reusableCellCount(int type) {
  if (type == k_resultCellType) {
    return k_numberOfReusableCells;
  }
  return 1;
}

int ResultsDataSource::typeAtIndex(int index) const {
  if (index == numberOfRows() - 1) {
    return k_buttonCellType;
  }
  return k_resultCellType;
}

}  // namespace Inference
