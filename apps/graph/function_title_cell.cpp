#include "function_title_cell.h"
#include <assert.h>

namespace Graph {

FunctionTitleCell::FunctionTitleCell() :
  EvenOddCell(),
  m_bufferTextView(0.5f, 0.5f)
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

void FunctionTitleCell::setOrientation(Orientation orientation) {
  m_orientation = orientation;
}

int FunctionTitleCell::numberOfSubviews() const {
  return 1;
}

View * FunctionTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_bufferTextView;
}

void FunctionTitleCell::layoutSubviews() {
  m_bufferTextView.setFrame(bounds());
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
