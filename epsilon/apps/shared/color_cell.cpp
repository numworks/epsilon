#include "color_cell.h"

namespace Shared {

void ColorView::drawRect(KDContext* ctx, KDRect rect) const {
  KDColor workingBuffer[k_dotDiameter * k_dotDiameter];
  ctx->blendRectWithMask(bounds(), m_color, Dots::Mask(k_dotSize, false),
                         workingBuffer);
}

KDSize ColorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_dotDiameter, k_dotDiameter);
}

void ColorView::setColor(KDColor color) {
  if (color != m_color) {
    m_color = color;
    markWholeFrameAsDirty();
  }
}

}  // namespace Shared
