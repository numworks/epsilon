#include "function_expression_cell.h"
#include <assert.h>

using namespace Poincare;

namespace Shared {

FunctionExpressionCell::FunctionExpressionCell() :
  EvenOddCell(),
  m_expressionView()
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
  if (highlight != EvenOddCell::isHighlighted()) {
    EvenOddCell::setHighlighted(highlight);
    m_expressionView.setBackgroundColor(backgroundColor());
  }
}

int FunctionExpressionCell::numberOfSubviews() const {
  return 1;
}

View * FunctionExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void FunctionExpressionCell::layoutSubviews() {
  KDRect expressionFrame(k_separatorThickness+k_margin, 0, bounds().width() - k_separatorThickness-k_margin, bounds().height()-k_separatorThickness);
  m_expressionView.setFrame(expressionFrame);
}

void FunctionExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
  // Color the vertical separator
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), Palette::GreyBright);
  // Color the horizontal separator
  ctx->fillRect(KDRect(k_separatorThickness, bounds().height()-k_separatorThickness, bounds().width()-k_separatorThickness, k_separatorThickness), separatorColor);
  // Color the margin
  ctx->fillRect(KDRect(k_separatorThickness, 0, k_margin, bounds().height()-k_separatorThickness), backgroundColor());
}

}
