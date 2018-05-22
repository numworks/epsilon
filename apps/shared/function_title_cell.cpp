#include "function_title_cell.h"
#include <assert.h>

namespace Shared {

FunctionTitleCell::FunctionTitleCell(Orientation orientation) :
  EvenOddCell(),
  m_orientation(orientation)
{
}

void FunctionTitleCell::setColor(KDColor color) {
  m_functionColor = color;
  reloadCell();
}

void FunctionTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_orientation == Orientation::VerticalIndicator){
    ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()), m_functionColor);
    // Color the vertical separator
    ctx->fillRect(KDRect(bounds().width()-k_separatorThickness, 0, k_separatorThickness, bounds().height()), Palette::GreyBright);
    KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
    // Color the horizontal separator
    ctx->fillRect(KDRect(k_colorIndicatorThickness, bounds().height()-k_separatorThickness, bounds().width()-k_colorIndicatorThickness-k_separatorThickness, k_separatorThickness), separatorColor);
  } else {
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
  }
}

}
