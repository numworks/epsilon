#include "function_expression_view.h"
#include <assert.h>

namespace Graph {

constexpr KDColor FunctionExpressionView::k_separatorColor;

FunctionExpressionView::FunctionExpressionView() :
  EvenOddCell(),
  m_function(nullptr),
  m_expressionView(ExpressionView())
{
}

void FunctionExpressionView::setFunction(Function * f) {
  m_function = f;
  m_expressionView.setExpression(m_function->layout());
}

void FunctionExpressionView::reloadCell() {
  EvenOddCell::reloadCell();
  m_expressionView.setBackgroundColor(backgroundColor());
  if (m_function) {
    bool active = m_function->isActive();
    KDColor textColor = active ? KDColorBlack : Palette::GreyDark;
    m_expressionView.setTextColor(textColor);
  }
}

Function * FunctionExpressionView::function() {
  return m_function;
}

int FunctionExpressionView::numberOfSubviews() const {
  return 1;
}

View * FunctionExpressionView::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void FunctionExpressionView::layoutSubviews() {
  KDRect expressionFrame(k_separatorThickness, 0, bounds().width() - k_separatorThickness, bounds().height());
  m_expressionView.setFrame(expressionFrame);
}

void FunctionExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Color the separator
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), k_separatorColor);
}

}
