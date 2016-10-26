#include "history_view_cell.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

HistoryViewCell::HistoryViewCell() :
  Responder(nullptr),
  m_prettyPrint(PrettyPrintView(this)),
  m_result(BufferTextView(1.0f, 0.5f))
{
}

int HistoryViewCell::numberOfSubviews() const {
  return 2;
}

View * HistoryViewCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_prettyPrint;
    case 1:
      return &m_result;
    default:
      assert(false);
      return nullptr;
  }
}

void HistoryViewCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  // Position the result
  KDSize prettyPrintSize = m_prettyPrint.minimalSizeForOptimalDisplay();
  KDRect prettyPrintFrame(0, 0, width, prettyPrintSize.height());
  m_prettyPrint.setFrame(prettyPrintFrame);
  KDRect resultFrame(0, prettyPrintSize.height(), width, height - prettyPrintSize.height());
  m_result.setFrame(resultFrame);
}

void HistoryViewCell::setCalculation(Calculation * calculation) {
  m_prettyPrint.setExpression(calculation->layout());
  char buffer[7];
  Float(calculation->evaluation()).convertFloatToText(buffer, 14, 7);
  m_result.setText(buffer);
}

void HistoryViewCell::reloadCell() {
  KDColor backgroundColor = isHighlighted() ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_result.setBackgroundColor(backgroundColor);
  m_prettyPrint.setBackgroundColor(backgroundColor);
  TableViewCell::reloadCell();
  layoutSubviews();
}

}
