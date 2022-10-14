#include "memoized_curve_view_range.h"
#include <cmath>
#include <assert.h>
#include <ion.h>

using namespace Poincare;

namespace Shared {

void MemoizedCurveViewRange::privateSet(float f, float limit, Range1D & range1D, void (Range1D::*setter)(float, float), bool updateGridUnit, float * gridUnit) {
  (range1D.*setter)(f, limit);
  *gridUnit = updateGridUnit ? (gridUnit == &m_xGridUnit ? CurveViewRange::xGridUnit() : CurveViewRange::yGridUnit()) : 0.f;
}

}
