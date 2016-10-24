#include "history_view_cell.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

HistoryViewCell::HistoryViewCell() :
  m_calculation(nullptr),
  m_result(BufferTextView(1.0f, 0.5f))
{
}

int HistoryViewCell::numberOfSubviews() const {
  return 1;
}

View * HistoryViewCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_result;
}

void HistoryViewCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  // Position the result
  KDSize prettyPrintSize = KDSize(0,0);
  if (m_calculation && m_calculation->layout() != nullptr) {
    prettyPrintSize = m_calculation->layout()->size();
  }
  KDRect resultFrame(prettyPrintSize.width(), 0, width - prettyPrintSize.width(), height);
  m_result.setFrame(resultFrame);
}

void HistoryViewCell::setCalculation(Calculation * calculation) {
  m_calculation = calculation;
  char buffer[7];
  m_calculation->evaluation()->convertFloatToText(buffer, 14, 7);
  m_result.setText(buffer);
}

void HistoryViewCell::reloadCell() {
  KDColor backgroundColor = isHighlighted() ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_result.setBackgroundColor(backgroundColor);
  TableViewCell::reloadCell();
}

void HistoryViewCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Select background color
  KDColor backgroundColor = isHighlighted() ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  ctx->fillRect(rect, backgroundColor);
  // Draw the pretty print
  if (m_calculation && m_calculation->layout() != nullptr) {
    m_calculation->layout()->draw(ctx, KDPointZero, KDColorBlack, backgroundColor);
  }
}

}
