#include "function_name_view.h"

FunctionNameView::FunctionNameView() :
  FunctionCell()
{
}

void FunctionNameView::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // First color the color indicator
  KDCoordinate height = bounds().height();
  KDColor functionColor = m_function->color();
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, height), functionColor);
  // Select function name color and the text color according to the state of the function
  bool active = m_function->isActive();
  KDColor textColor = active ? KDColorBlack : FunctionCell::k_desactiveTextColor;
  KDColor functionNameColor = active ? functionColor : FunctionCell::k_desactiveTextColor;
  // Select the background color according to the even line and the cursor selection
  KDColor background = backgroundColor();
  // Position the name of the function
  const char * functionName = m_function->name();
  KDCoordinate baseline = m_function->layout()->baseline();
  KDSize nameSize = KDText::stringSize(functionName);
  ctx->drawString(functionName, KDPoint(k_colorIndicatorThickness, baseline-nameSize.height()), functionNameColor, background);
  ctx->drawString("(x)", KDPoint(k_colorIndicatorThickness+nameSize.width(), baseline-nameSize.height()), textColor, background);
}
