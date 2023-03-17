#include "color_cell.h"

#include "dots.h"

namespace Shared {

void ColorView::drawRect(KDContext *ctx, KDRect rect) const {
  KDColor workingBuffer[Dots::LargeDotDiameter * Dots::LargeDotDiameter];
  ctx->blendRectWithMask(bounds(), m_color,
                         reinterpret_cast<const uint8_t *>(Dots::LargeDotMask),
                         workingBuffer);
}

KDSize ColorView::minimalSizeForOptimalDisplay() const {
  return KDSize(Dots::LargeDotDiameter, Dots::LargeDotDiameter);
}

void ColorView::setColor(KDColor color) {
  if (color != m_color) {
    m_color = color;
    markRectAsDirty(bounds());
  }
}

}  // namespace Shared
