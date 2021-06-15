#include "function_title_cell.h"
#include <assert.h>
#include <algorithm>

using namespace Escher;

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
    // Draw the color indicator
    ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()), m_functionColor);
  } else {
    // Draw the color indicator
    ctx->fillRect(KDRect(0, 0, bounds().width(), k_colorIndicatorThickness), m_functionColor);
  }
}

KDRect FunctionTitleCell::subviewFrame() const {
  if (m_orientation == Orientation::VerticalIndicator) {
    return KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness, bounds().height());
  }
  return KDRect(0, k_colorIndicatorThickness, bounds().width(), bounds().height()-k_colorIndicatorThickness);
}

float FunctionTitleCell::verticalAlignment() const {
  assert(m_orientation == Orientation::VerticalIndicator);
  return std::max(
      0.0f,
      std::min(
        1.0f,
        m_baseline < 0 ? KDContext::k_alignCenter : verticalAlignmentGivenExpressionBaselineAndRowHeight(m_baseline, subviewFrame().height())));
}

}
