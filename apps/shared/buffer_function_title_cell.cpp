#include "buffer_function_title_cell.h"
#include <assert.h>

namespace Shared {

BufferFunctionTitleCell::BufferFunctionTitleCell(Orientation orientation, const KDFont * font) :
  FunctionTitleCell(orientation),
  m_bufferTextView(font, 0.5f, k_verticalAlignment)
{
}

void BufferFunctionTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_bufferTextView.setHighlighted(highlight);
}

void BufferFunctionTitleCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_bufferTextView.setEven(even);
}

void BufferFunctionTitleCell::setColor(KDColor color) {
  FunctionTitleCell::setColor(color);
  m_bufferTextView.setTextColor(color);
}

void BufferFunctionTitleCell::setText(const char * title) {
  m_bufferTextView.setText(title);
}

int BufferFunctionTitleCell::numberOfSubviews() const {
  return 1;
}

View * BufferFunctionTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void BufferFunctionTitleCell::layoutSubviews(bool force) {
  m_bufferTextView.setFrame(bufferTextViewFrame(), force);
}

KDRect BufferFunctionTitleCell::bufferTextViewFrame() const {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(), bounds().height() - k_colorIndicatorThickness);
  if (m_orientation == Orientation::VerticalIndicator){
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness-k_separatorThickness, bounds().height()-k_separatorThickness);
  }
  return textFrame;
}

}
