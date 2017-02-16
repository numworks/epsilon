#include "function_expression_cell.h"
#include <assert.h>

using namespace Poincare;

namespace Graph {

FunctionExpressionCell::FunctionExpressionCell() :
  EvenOddCell(),
  m_expressionView(ExpressionView())
{
}

void FunctionExpressionCell::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpression(expressionLayout);
}

void FunctionExpressionCell::setTextColor(KDColor textColor) {
  m_expressionView.setTextColor(textColor);
}

void FunctionExpressionCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_expressionView.setBackgroundColor(backgroundColor());
}

void FunctionExpressionCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_expressionView.setBackgroundColor(backgroundColor());
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
  // Color the separator
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), Palette::GreyBright);
}

}
