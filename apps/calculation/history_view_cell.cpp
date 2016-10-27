#include "history_view_cell.h"
#include <assert.h>
#include <string.h>

namespace Calculation {

HistoryViewCell::HistoryViewCell() :
  Responder(nullptr),
  m_prettyPrint(PrettyPrintView(this)),
  m_result(BufferTextView(1.0f, 0.5f)),
  m_selectedView(HistoryViewCell::SelectedView::Result)
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
  if (m_selectedView == HistoryViewCell::SelectedView::Result) {
    m_result.setBackgroundColor(backgroundColor);
    m_prettyPrint.setBackgroundColor(Palette::CellBackgroundColor);
  } else {
    m_result.setBackgroundColor(Palette::CellBackgroundColor);
    m_prettyPrint.setBackgroundColor(backgroundColor);
  }
  TableViewCell::reloadCell();
  layoutSubviews();
}

void HistoryViewCell::didBecomeFirstResponder() {
  if (m_selectedView == HistoryViewCell::SelectedView::Result) {
    //app()->setFirstResponder(&m_result);
    return;
  }
  app()->setFirstResponder(&m_prettyPrint);
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      if (m_selectedView == HistoryViewCell::SelectedView::PrettyPrint) {
        m_selectedView = HistoryViewCell::SelectedView::Result;
        //app()->setFirstResponder(&m_result);
        reloadCell();
        return true;
      }
      return false;
    case Ion::Events::Event::UP_ARROW:
      if (m_selectedView == HistoryViewCell::SelectedView::Result) {
        m_selectedView = HistoryViewCell::SelectedView::PrettyPrint;
        app()->setFirstResponder(&m_prettyPrint);
        reloadCell();
        return true;
      }
      return false;
    default:
      return false;
  }
}

}
