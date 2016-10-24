#include "function_expression_view.h"

namespace Graph {

constexpr KDColor FunctionExpressionView::k_separatorColor;

FunctionExpressionView::FunctionExpressionView() :
  EvenOddCell(),
  m_function(nullptr)
{
}

void FunctionExpressionView::setFunction(Function * f) {
  m_function = f;
  markRectAsDirty(bounds());
}

Function * FunctionExpressionView::function() {
  return m_function;
}

void FunctionExpressionView::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Color the separator
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, bounds().height()), k_separatorColor);
  if (m_function->layout() == nullptr) {
    return;
  }
  // Select the background color according to the even line and the cursor selection
  KDColor background = backgroundColor();
  // Select text color according to the state of the function
  bool active = m_function->isActive();
  KDColor text = active ? KDColorBlack : Palette::k_desactiveTextColor;
  //Position the origin of expression
  KDSize expressionSize = m_function->layout()->size();
  KDPoint origin(k_separatorThickness, 0.5f*(m_frame.height() - expressionSize.height()));
  m_function->layout()->draw(ctx, origin, text, background);
}

}
