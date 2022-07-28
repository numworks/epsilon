#include "vector_model.h"
#include "vector_graph_cell.h"
#include <algorithm>

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
  float ratio = verticallyCapped ? height/k_height : width/k_width;

  /* When the vector points downward, we need to keep space around the origin
   * for the arc which extends on the other side of the vector. */
  float arcHorizontalMargin = x > 0 && y < 0 && !verticallyCapped ? VectorGraphView::k_arcRadiusInPixels * ratio : 0.f;
  float arcVerticalMargin = y < 0 && verticallyCapped ? VectorGraphView::k_arcRadiusInPixels * ratio : 0.f;

  width += arcHorizontalMargin;
  height += arcVerticalMargin;

  /* Recompute ratio with updated size */
  verticallyCapped = height * k_xyRatio > width;
  ratio = verticallyCapped ? height/k_height : width/k_width;

  float xMargin = k_marginInPixels*ratio + (verticallyCapped ? (height * k_xyRatio - width) / 2.f : 0.f);
  float yMargin = k_marginInPixels*ratio + (verticallyCapped ? 0.f : (width / k_xyRatio - height) / 2.f);

  setXMin(std::min(-arcHorizontalMargin, x) - xMargin);
  setXMax(std::max(0.f, x) + xMargin);
  setYMin(std::min(0.f, y) - yMargin);
  setYMax(std::max(arcVerticalMargin, y) + yMargin);
}

}
