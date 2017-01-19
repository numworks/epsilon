#include "function_title_cell.h"
#include <assert.h>

namespace Graph {

FunctionTitleCell::FunctionTitleCell(Orientation orientation, KDText::FontSize size) :
  EvenOddCell(),
  m_bufferTextView(size, 0.5f, 0.5f),
  m_orientation(orientation)
{
}

void FunctionTitleCell::reloadCell() {
  EvenOddCell::reloadCell();
  m_bufferTextView.setBackgroundColor(backgroundColor());
}

void FunctionTitleCell::setText(const char * title) {
  m_bufferTextView.setText(title);
}

void FunctionTitleCell::setColor(KDColor color) {
  m_functionColor = color;
  m_bufferTextView.setTextColor(color);
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
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness, bounds().height());
  }
  m_bufferTextView.setFrame(textFrame);
}

void FunctionTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  if (m_orientation == Orientation::VerticalIndicator){
    ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()), m_functionColor);
  } else {
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
  }
}

}
