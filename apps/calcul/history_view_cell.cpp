#include "history_view_cell.h"
#include <assert.h>
#include <string.h>

namespace Calcul {

HistoryViewCell::HistoryViewCell() :
  m_calcul(nullptr),
  m_highlighted(false),
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
  if (m_calcul && m_calcul->layout() != nullptr) {
    prettyPrintSize = m_calcul->layout()->size();
  }
  KDRect resultFrame(prettyPrintSize.width(), 0, width - prettyPrintSize.width(), height);
  m_result.setFrame(resultFrame);
}

void HistoryViewCell::setCalcul(Calcul * calcul) {
  m_calcul = calcul;
  char buffer[7];
  m_calcul->evaluation()->convertFloatToText(buffer, 14, 7);
  m_result.setText(buffer);
}

void HistoryViewCell::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  KDColor backgroundColor = m_highlighted ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_result.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

void HistoryViewCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Select background color
  KDColor backgroundColor = m_highlighted ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  ctx->fillRect(rect, backgroundColor);
  // Draw the pretty print
  if (m_calcul && m_calcul->layout() != nullptr) {
    m_calcul->layout()->draw(ctx, KDPointZero, KDColorBlack, backgroundColor);
  }
}

}
