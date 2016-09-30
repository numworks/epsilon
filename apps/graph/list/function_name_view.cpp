#include "function_name_view.h"

FunctionNameView::FunctionNameView() :
  FunctionCell()
{
}

void FunctionNameView::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  // First color the color indicator
  KDColor functionColor = m_function->color();
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, height), functionColor);
  // Select the background color according to the even line and the cursor selection
  bool evenLine = m_even;
  KDColor background = evenLine ? FunctionCell::k_evenLineBackgroundColor : FunctionCell::k_oddLineBackgroundColor;
  background = m_highlighted ? FunctionCell::k_selectedLineBackgroundColor : background;
  ctx->fillRect(KDRect(4, 0, width-4, height), background);
  // Select text color according to the state of the function
  bool active = m_function->isActive();
  KDColor text = active ? KDColorBlack : FunctionCell::k_desactiveTextColor;
  ctx->drawString(m_function->name(), KDPoint(4, 0), text, background);
  // m_function->layout()->draw(ctx, KDPointZero);
}
