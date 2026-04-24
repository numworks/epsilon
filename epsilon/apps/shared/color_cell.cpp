#include "color_cell.h"

#include <escher/theme/color_picker.h>

using namespace Escher;

namespace Shared {

void ColorView::drawRect(KDContext* ctx, KDRect rect) const {
  KDColor workingBuffer[k_dotDiameter * k_dotDiameter];
  ctx->blendRectWithMask(bounds(), ColorPicker::Resolve(m_color),
                         Dots::Mask(k_dotSize, false), workingBuffer);
}

KDSize ColorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_dotDiameter, k_dotDiameter);
}

void ColorView::setColor(ThemeComponent color) {
  if (color != m_color) {
    m_color = color;
    markWholeFrameAsDirty();
  }
}

}  // namespace Shared
