#include "vector_model.h"

#include <algorithm>

#include "vector_graph_cell.h"

namespace Calculation {

void VectorModel::setVector(float x, float y) {
  m_vector[0] = x;
  m_vector[1] = y;
  recomputeViewRange();
}

void VectorModel::recomputeViewRange() {
  const float x = m_vector[0];
  const float y = m_vector[1];

  float width = std::abs(x);
  float height = std::abs(y);
  bool verticallyCapped = height * k_xyRatio > width;
  float ratio = verticallyCapped ? height / k_height : width / k_width;

  /* When the vector points downward or left, we need to keep space around the
   * origin for the arc which extends on the other side of the vector. */
  float arcDefaultMargin = VectorGraphPolicy::k_arcRadiusInPixels * ratio;
  float arcLeftMargin =
      x > 0 && y < 0 && !verticallyCapped ? arcDefaultMargin : 0.f;
  float arcRightMargin = x < 0 && !verticallyCapped ? arcDefaultMargin : 0.f;
  float arcTopMargin = y < 0 && verticallyCapped ? arcDefaultMargin : 0.f;

  width += arcLeftMargin + arcRightMargin;
  height += arcTopMargin;

  /* Recompute ratio with updated size */
  verticallyCapped = height * k_xyRatio > width;
  ratio = verticallyCapped ? height / k_height : width / k_width;

  float xMargin = k_marginInPixels * ratio +
                  (verticallyCapped ? (height * k_xyRatio - width) / 2.f : 0.f);
  float yMargin = k_marginInPixels * ratio +
                  (verticallyCapped ? 0.f : (width / k_xyRatio - height) / 2.f);

  setXMin(std::min(-arcLeftMargin, x) - xMargin);
  setXMax(std::max(arcRightMargin, x) + xMargin);
  setYMin(std::min(0.f, y) - yMargin);
  setYMax(std::max(arcTopMargin, y) + yMargin);
}

}  // namespace Calculation
