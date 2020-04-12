#include "function_title_cell.h"
#include <assert.h>
#include <algorithm>

namespace Shared {

void FunctionTitleCell::setOrientation(Orientation orientation) {
  m_orientation = orientation;
  reloadCell();
}

void FunctionTitleCell::setColor(KDColor color) {
  m_functionColor = color;
  reloadCell();
}

void FunctionTitleCell::setBaseline(KDCoordinate baseline) {
  if (m_baseline != baseline) {
    m_baseline = baseline;
    reloadCell();
  }
}

void FunctionTitleCell::drawRect(KDContext * ctx, KDRect rect) const {
  if (m_orientation == Orientation::VerticalIndicator){
    KDColor separatorColor = m_even ? Palette::WallScreen : KDColorWhite;
    KDColor backgroundColor = m_even ? KDColorWhite : Palette::WallScreen;
    // Draw the color indicator
    ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()), m_functionColor);
    // Draw the horizontal separator
    ctx->fillRect(KDRect(k_colorIndicatorThickness, bounds().height()-k_separatorThickness, bounds().width()-k_colorIndicatorThickness, k_separatorThickness), separatorColor);
    // Draw some background
    ctx->fillRect(KDRect(bounds().width() - k_equalWidthWithMargins, 0, k_equalWidthWithMargins, bounds().height()-k_separatorThickness), backgroundColor);
    // Draw '='
    KDPoint p = KDPoint(bounds().width() - k_equalWidthWithMargins, m_baseline - font()->glyphSize().height()/2 - 1); // -1 is visually needed
    ctx->drawString("=", p, font(), m_functionColor, backgroundColor);
  } else {
    // Draw the color indicator
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
  }
}

KDRect FunctionTitleCell::subviewFrame() const {
  if (m_orientation == Orientation::VerticalIndicator) {
    return KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness - k_equalWidthWithMargins, bounds().height()-k_separatorThickness);
  }
  return KDRect(0, k_colorIndicatorThickness, bounds().width(), bounds().height()-k_colorIndicatorThickness);
}

float FunctionTitleCell::verticalAlignment() const {
  assert(m_orientation == Orientation::VerticalIndicator);
  return std::max(
      0.0f,
      std::min(
        1.0f,
        m_baseline < 0 ? 0.5f : verticalAlignmentGivenExpressionBaselineAndRowHeight(m_baseline, subviewFrame().height())));
}

}
