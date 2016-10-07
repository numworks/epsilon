#include "function_expression_view.h"

FunctionExpressionView::FunctionExpressionView() :
  FunctionCell()
{
}

void FunctionExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Select the background color according to the even line and the cursor selection
  KDColor background = backgroundColor();
  // Select text color according to the state of the function
  bool active = m_function->isActive();
  KDColor text = active ? KDColorBlack : FunctionCell::k_desactiveTextColor;
  m_function->layout()->draw(ctx, KDPointZero, text, background);
}
