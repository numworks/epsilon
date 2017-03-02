#include "function_title_cell.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

FunctionTitleCell::FunctionTitleCell(Orientation orientation, KDText::FontSize size) :
  Shared::FunctionTitleCell(orientation),
  m_bufferTextView(size, 0.5f, 0.5f)
{
}

void FunctionTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_bufferTextView.setHighlighted(highlight);
}

void FunctionTitleCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_bufferTextView.setEven(even);
}

void FunctionTitleCell::setColor(KDColor color) {
  Shared::FunctionTitleCell::setColor(color);
  m_bufferTextView.setTextColor(color);
}

void FunctionTitleCell::setText(const char * title) {
  m_bufferTextView.setText(title);
}

int FunctionTitleCell::numberOfSubviews() const {
  return 1;
}

View * FunctionTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void FunctionTitleCell::layoutSubviews() {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(), bounds().height() - k_colorIndicatorThickness);
  if (m_orientation == Orientation::VerticalIndicator){
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness, bounds().height()-k_separatorThickness);
  }
  m_bufferTextView.setFrame(textFrame);
}

}
