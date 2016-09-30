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
  KDColor backgroundColor = evenLine ? FunctionCell::k_evenLineBackgroundColor : FunctionCell::k_oddLineBackgroundColor;
  backgroundColor = m_highlighted ? FunctionCell::k_selectedLineBackgroundColor : backgroundColor;
  ctx->fillRect(KDRect(k_colorIndicatorThickness, 0, width-k_colorIndicatorThickness, height), backgroundColor);
  // Select function name color and the tex color according to the state of the function
  bool active = m_function->isActive();
  KDColor textColor = active ? KDColorBlack : FunctionCell::k_desactiveTextColor;
  KDColor functionNameColor = active ? functionColor : FunctionCell::k_desactiveTextColor;
  // Position the name of the function
  const char * functionName = m_function->name();
  KDCoordinate baseline = m_function->layout()->baseline();
  KDSize nameSize = KDText::stringSize(functionName);
  ctx->drawString(functionName, KDPoint(k_colorIndicatorThickness, baseline-nameSize.height()), functionNameColor, backgroundColor);
  ctx->drawString("(x)", KDPoint(k_colorIndicatorThickness+nameSize.width(), baseline-nameSize.height()), textColor, backgroundColor);
}
