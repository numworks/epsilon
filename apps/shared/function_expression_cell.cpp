#include "function_expression_cell.h"
#include <assert.h>

namespace Shared {

FunctionExpressionCell::FunctionExpressionCell() :
  EvenOddCell(),
  m_function(nullptr),
  m_expressionView(EvenOddExpressionCell())
{
}

void FunctionExpressionCell::setFunction(Function * f) {
  m_function = f;
  m_expressionView.setExpression(m_function->layout());
}

void FunctionExpressionCell::reloadCell() {
  EvenOddCell::reloadCell();
  if (m_function) {
    bool active = m_function->isActive();
    KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
    m_expressionView.setTextColor(textColor);
  }
}

void FunctionExpressionCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_expressionView.setEven(even);
}

void FunctionExpressionCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_expressionView.setHighlighted(highlight);
}

Function * FunctionExpressionCell::function() {
  return m_function;
}

int FunctionExpressionCell::numberOfSubviews() const {
  return 1;
}

View * FunctionExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void FunctionExpressionCell::layoutSubviews() {
  KDRect expressionFrame(k_separatorThickness, 0, bounds().width() - k_separatorThickness, bounds().height());
  m_expressionView.setFrame(expressionFrame);
}

void FunctionExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Color the separator
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), Palette::GreyBright);
}

}
