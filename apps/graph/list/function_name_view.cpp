#include "function_name_view.h"
#include "../function_store.h"

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
  KDSize textSize = KDText::stringSize(functionName);
  KDSize expressionSize = m_function->layout()->size();
  KDPoint origin(0.5f*(k_colorIndicatorThickness + m_frame.width() - 4*textSize.width()),
    baseline-textSize.height()+0.5f*(m_frame.height() - expressionSize.height()));
  ctx->drawString(functionName, origin, functionNameColor, background);
  ctx->drawString(Graph::Function::Parameter, origin.translatedBy(KDPoint(textSize.width(), 0)), textColor, background);
}
