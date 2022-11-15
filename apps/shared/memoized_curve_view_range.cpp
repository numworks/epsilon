#include "memoized_curve_view_range.h"
#include <cmath>
#include <assert.h>
#include <ion.h>

using namespace Poincare;

namespace Shared {

MemoizedCurveViewRange::MemoizedCurveViewRange() :
  /* FIXME Due to some impromptu reload, some controllers use the range before
   * setting its values, making it necessary to initialize it with some
   * contrived values. This should probably be reworked. */
  m_range(Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength),
  m_xGridUnit(k_defaultGridUnit),
  m_yGridUnit(k_defaultGridUnit)
{}

void MemoizedCurveViewRange::protectedSetX(Poincare::Range1D x, float limit) {
  protectedSetXMin(x.min(), false, limit);
  protectedSetXMax(x.max(), true, limit);
}

void MemoizedCurveViewRange::protectedSetY(Poincare::Range1D y, float limit) {
  protectedSetYMin(y.min(), false, limit);
  protectedSetYMax(y.max(), true, limit);
}

void MemoizedCurveViewRange::privateSet(float f, float limit, Range1D * range1D, void (Range1D::*setter)(float, float), bool updateGridUnit, float * gridUnit) {
  (range1D->*setter)(f, limit);
  *gridUnit = updateGridUnit ? (gridUnit == &m_xGridUnit ? CurveViewRange::xGridUnit() : CurveViewRange::yGridUnit()) : 0.f;
}

}
