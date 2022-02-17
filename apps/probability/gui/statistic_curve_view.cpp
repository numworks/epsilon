#include "statistic_curve_view.h"
#include <poincare/print_float.h>
#include <cmath>

namespace Probability {

constexpr KDColor StatisticCurveView::k_backgroundColor;


void StatisticCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);
}

char * StatisticCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

KDCoordinate StatisticCurveView::drawGraduationAtPosition(KDContext * ctx, float horizontalPosition, float verticalPosition, KDColor color, KDCoordinate halfHeight, KDCoordinate axisThickness) const {
  float verticalOrigin = std::round(floatToPixel(Axis::Vertical, verticalPosition));
  KDCoordinate graduationPosition = std::round(floatToPixel(Axis::Horizontal, horizontalPosition));
  // Graduation
  KDRect graduation = KDRect(graduationPosition,
                             verticalOrigin - halfHeight,
                             axisThickness,
                             halfHeight * 2 + axisThickness );
  ctx->fillRect(graduation, color);
  return graduationPosition;
}

}  // namespace Probability
