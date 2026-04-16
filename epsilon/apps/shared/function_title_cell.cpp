#include "function_title_cell.h"

#include <escher/theme/color_picker.h>

namespace Shared {

void FunctionTitleCell::setColor(Escher::ThemeComponent color) {
  m_functionColor = color;
  reloadCell();
}

void FunctionTitleCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Draw the color indicator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness),
                Escher::ColorPicker::Resolve(m_functionColor));
}

KDRect FunctionTitleCell::subviewFrame() const {
  return KDRect(0, k_colorIndicatorThickness, bounds().width(),
                bounds().height() - k_colorIndicatorThickness);
}

}  // namespace Shared
