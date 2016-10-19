#include "function_title_cell.h"
#include "../function.h"

namespace Graph {

void FunctionTitleCell::setColor(KDColor color) {
  m_functionColor = color;
}

void FunctionTitleCell::setDerivative(bool derivative) {
  m_derivative = derivative;
}

void FunctionTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Write the "(x)"
  KDColor background = backgroundColor();
  KDSize textSize = KDText::stringSize("f");
  KDPoint origin(0.5f*(m_frame.width() - textSize.width()), 0.5f*(m_frame.height() - textSize.height()));
  if (m_derivative) {
    ctx->drawString("'", origin.translatedBy(KDPoint(textSize.width(), 0)), m_functionColor , background);
    ctx->drawString(Function::Parameter, origin.translatedBy(KDPoint(2*textSize.width(), 0)), m_functionColor , background);
  } else {
    ctx->drawString(Function::Parameter, origin.translatedBy(KDPoint(textSize.width(), 0)), m_functionColor , background);
  }
  // Color the color indicator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
}

}
