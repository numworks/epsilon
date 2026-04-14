#include "function_title_cell.h"

#include <apps/global_preferences.h>

namespace Shared {

void FunctionTitleCell::setColor(Escher::ThemeComponent color) {
  m_functionColor = color;
  reloadCell();
}

void FunctionTitleCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Draw the color indicator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness),
                SharedGlobalPrefs().color(m_functionColor));
}

KDRect FunctionTitleCell::subviewFrame() const {
  return KDRect(0, k_colorIndicatorThickness, bounds().width(),
                bounds().height() - k_colorIndicatorThickness);
}

}  // namespace Shared
