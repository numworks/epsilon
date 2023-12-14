#include "memoized_curve_view_range.h"

#include <assert.h>
#include <ion.h>

#include <cmath>

using namespace Poincare;

namespace Shared {

MemoizedCurveViewRange::MemoizedCurveViewRange()
    : /* FIXME Due to some impromptu reload, some controllers use the range
       * before setting its values, making it necessary to initialize it with
       * some contrived values. This should probably be reworked. */
      m_range(Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength,
              Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength),
      m_xGridUnit(k_defaultGridUnit),
      m_yGridUnit(k_defaultGridUnit) {}

void MemoizedCurveViewRange::privateSet(float min, float max, float limit,
                                        bool x) {
  Range1D* range1D = x ? m_range.x() : m_range.y();
  *range1D = Range1D::ValidRangeBetween(min, max, limit);
  if (x) {
    m_xGridUnit = CurveViewRange::xGridUnit();
  } else {
    m_yGridUnit = CurveViewRange::yGridUnit();
  }
}

}  // namespace Shared
