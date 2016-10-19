#include "function_title_cell.h"
#include "../function.h"

namespace Graph {

void FunctionTitleCell::setColor(KDColor color) {
  m_functionColor = color;
}

void FunctionTitleCell::setDerivative(bool derivative) {
  m_derivative = derivative;
  PointerTextView * pointerTextView = (PointerTextView *)subviewAtIndex(0);
  int textwidth = KDText::stringSize("f").width();
  // Here we compute the right horizontal alignment to center "f(x)" or "f'(x)"
  if (derivative) {
    pointerTextView->setAlignment(0.5f*(m_frame.width() - 5*textwidth)/(m_frame.width() - textwidth), 0.5f);
  } else {
    pointerTextView->setAlignment(0.5f*(m_frame.width() - 4*textwidth)/(m_frame.width() - textwidth), 0.5f);
  }
}

void FunctionTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Write the "(x)"
  KDColor background = backgroundColor();
  KDSize textSize = KDText::stringSize("f");
  KDPoint origin(0.5f*(m_frame.width() - 4*textSize.width()), 0.5f*(m_frame.height() - textSize.height()));
  if (m_derivative) {
    origin = KDPoint(0.5f*(m_frame.width() - 5*textSize.width()), 0.5f*(m_frame.height() - textSize.height()));
    ctx->drawString("'", origin.translatedBy(KDPoint(textSize.width(), 0)), m_functionColor , background);
    ctx->drawString(Function::Parameter, origin.translatedBy(KDPoint(2*textSize.width(), 0)), m_functionColor , background);
  } else {
    ctx->drawString(Function::Parameter, origin.translatedBy(KDPoint(textSize.width(), 0)), m_functionColor , background);
  }
  // Color the color indicator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
}

}
