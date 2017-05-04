#include "color_view.h"

namespace HardwareTest {

ColorView::ColorView() :
  m_fillColor(KDColorRed),
  m_outlineColor(KDColorGreen)
{
}

void ColorView::setColors(KDColor fillColor, KDColor outlineColor) {
  if (m_fillColor != fillColor || m_outlineColor != outlineColor) {
    m_fillColor = fillColor;
    m_outlineColor = outlineColor;
    markRectAsDirty(bounds());
  }
}

KDColor ColorView::fillColor() {
  return m_fillColor;
}

KDColor ColorView::outlineColor() {
  return m_outlineColor;
}

void ColorView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, m_outlineColor);
  ctx->fillRect(KDRect(k_outlineThickness, k_outlineThickness, bounds().width()-2*k_outlineThickness, bounds().height()-2*k_outlineThickness), m_fillColor);
}

}
