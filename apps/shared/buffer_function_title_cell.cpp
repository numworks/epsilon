#include "buffer_function_title_cell.h"

#include <assert.h>

using namespace Escher;

namespace Shared {

BufferFunctionTitleCell::BufferFunctionTitleCell(KDFont::Size font)
    : FunctionTitleCell(),
      m_bufferTextView(font, KDContext::k_alignCenter, k_verticalAlignment) {}

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

void BufferFunctionTitleCell::setText(const char* title) {
  m_bufferTextView.setText(title);
}

int BufferFunctionTitleCell::numberOfSubviews() const { return 1; }

View* BufferFunctionTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void BufferFunctionTitleCell::layoutSubviews(bool force) {
  m_bufferTextView.setFrame(bufferTextViewFrame(), force);
}

KDRect BufferFunctionTitleCell::bufferTextViewFrame() const {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(),
                   bounds().height() - k_colorIndicatorThickness);
  return textFrame;
}

}  // namespace Shared
