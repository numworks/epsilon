#include "memoized_curve_view_range.h"

#include <assert.h>
#include <ion.h>

#include <cmath>

#include "poincare/pool_object.h"

using namespace Poincare;

namespace Shared {

MemoizedCurveViewRange::MemoizedCurveViewRange()
    : /* FIXME Due to some impromptu reload, some controllers use the range
       * before setting its values, making it necessary to initialize it with
       * some contrived values. This should probably be reworked. */
      m_range(-Range1D<float>::k_defaultHalfLength,
              Range1D<float>::k_defaultHalfLength,
              -Range1D<float>::k_defaultHalfLength,
              Range1D<float>::k_defaultHalfLength) {}

ExpressionOrFloat MemoizedCurveViewRange::xGridUnit() {
  if (std::isnan(static_cast<float>(m_gridUnit(OMG::Axis::Horizontal)))) {
    m_gridUnit.set(OMG::Axis::Horizontal,
                   computeGridUnit(OMG::Axis::Horizontal));
  }
  assert(static_cast<float>(m_gridUnit(OMG::Axis::Horizontal)) != 0.0f);
  return m_gridUnit(OMG::Axis::Horizontal);
}

ExpressionOrFloat MemoizedCurveViewRange::yGridUnit() {
  if (std::isnan(static_cast<float>(m_gridUnit(OMG::Axis::Vertical)))) {
    m_gridUnit.set(OMG::Axis::Vertical, computeGridUnit(OMG::Axis::Vertical));
  }
  assert(static_cast<float>(m_gridUnit(OMG::Axis::Vertical)) != 0.0f);
  return m_gridUnit(OMG::Axis::Vertical);
}

void MemoizedCurveViewRange::privateSet(float min, float max, float limit,
                                        bool x) {
  Range1D<float>* range1D = x ? m_range.x() : m_range.y();
  *range1D = Range1D<float>::ValidRangeBetween(min, max, limit);
  /* The grid unit has to be recomputed when the range changes. We must reset
   * both grid units because with normalization they can depend on each other.
   */
  resetGridUnit();
}

}  // namespace Shared
