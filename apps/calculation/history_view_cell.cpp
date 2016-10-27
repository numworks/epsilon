#include "history_view_cell.h"
#include "../constant.h"
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

KDColor HistoryViewCell::backgroundColor() const {
  KDColor background = m_even ? EvenOddCell::k_evenLineBackgroundColor : EvenOddCell::k_oddLineBackgroundColor;
  return background;
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
  if (prettyPrintSize.width() + k_digitHorizontalMargin > width) {
    m_prettyPrint.setFrame(KDRect(k_digitHorizontalMargin, k_digitVerticalMargin, width - k_digitHorizontalMargin, prettyPrintSize.height()));
  } else {
    m_prettyPrint.setFrame(KDRect(k_digitHorizontalMargin, k_digitVerticalMargin, prettyPrintSize.width(), prettyPrintSize.height()));
  }
  KDSize resultSize = m_result.minimalSizeForOptimalDisplay();
  KDRect resultFrame(width - resultSize.width() - k_digitHorizontalMargin, prettyPrintSize.height() + 2*k_digitVerticalMargin, resultSize.width(), height - prettyPrintSize.height() - 2*k_digitVerticalMargin);
  m_result.setFrame(resultFrame);
}

void HistoryViewCell::setCalculation(Calculation * calculation) {
  m_prettyPrint.setExpression(calculation->layout());
  char buffer[Constant::FloatBufferSizeInScientificMode];
  Float(calculation->evaluation()).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_result.setText(buffer);
}

void HistoryViewCell::reloadCell() {
  m_result.setBackgroundColor(backgroundColor());
  m_prettyPrint.setBackgroundColor(backgroundColor());
  if (isHighlighted()) {
    if (m_selectedView == HistoryViewCell::SelectedView::Result) {
      m_result.setBackgroundColor(Palette::FocusCellBackgroundColor);
    } else {
      m_prettyPrint.setBackgroundColor(Palette::FocusCellBackgroundColor);
    }
  }
  layoutSubviews();
  EvenOddCell::reloadCell();
  m_prettyPrint.reloadCell();
}

void HistoryViewCell::didBecomeFirstResponder() {
  if (m_selectedView == HistoryViewCell::SelectedView::PrettyPrint) {
    app()->setFirstResponder(&m_prettyPrint);
  }
}

HistoryViewCell::SelectedView HistoryViewCell::selectedView() {
  return m_selectedView;
}

void HistoryViewCell::setSelectedView(HistoryViewCell::SelectedView selectedView) {
  m_selectedView = selectedView;
}

bool HistoryViewCell::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      if (m_selectedView == HistoryViewCell::SelectedView::PrettyPrint) {
        m_selectedView = HistoryViewCell::SelectedView::Result;
        app()->setFirstResponder(this);
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
