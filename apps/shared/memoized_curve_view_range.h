#ifndef SHARED_MEMOIZED_CURVE_VIEW_RANGE_H
#define SHARED_MEMOIZED_CURVE_VIEW_RANGE_H

#include "curve_view_range.h"
#include <poincare/range.h>

namespace Shared {

class MemoizedCurveViewRange : public CurveViewRange {
public:
  MemoizedCurveViewRange();

  //CurveViewRange
  float xMin() const override { return m_range.xMin(); }
  float xMax() const override { return m_range.xMax(); }
  float yMin() const override { return m_range.yMin(); }
  float yMax() const override { return m_range.yMax(); }
  /* A null gridUnit value means a limit has been changed without updating the
   * grid unit. */
  float xGridUnit() const override { assert(m_xGridUnit != 0.0f); return m_xGridUnit; }
  float yGridUnit() const override { assert(m_yGridUnit != 0.0f); return m_yGridUnit; }
  virtual void setXMin(float f) { protectedSetXMin(f); }
  virtual void setXMax(float f) { protectedSetXMax(f); }
  virtual void setYMin(float f) { protectedSetYMin(f); }
  virtual void setYMax(float f) { protectedSetYMax(f); }

protected:
  /* A false updateGridUnit flag prevents computing grid_unit twice, and will
   * invalidate, so we can check if it has been properly updated later. */
  Poincare::Range2D memoizedRange() const { return m_range; }
  void protectedSetXMin(float f, bool updateGridUnit = true, float limit = Poincare::Range1D::k_maxFloat) { privateSet(f, limit, m_range.x(), &Poincare::Range1D::setMin, updateGridUnit, &m_xGridUnit); }
  void protectedSetXMax(float f, bool updateGridUnit = true, float limit = Poincare::Range1D::k_maxFloat) { privateSet(f, limit, m_range.x(), &Poincare::Range1D::setMax, updateGridUnit, &m_xGridUnit); }
  void protectedSetYMin(float f, bool updateGridUnit = true, float limit = Poincare::Range1D::k_maxFloat) { privateSet(f, limit, m_range.y(), &Poincare::Range1D::setMin, updateGridUnit, &m_yGridUnit); }
  void protectedSetYMax(float f, bool updateGridUnit = true, float limit = Poincare::Range1D::k_maxFloat) { privateSet(f, limit, m_range.y(), &Poincare::Range1D::setMax, updateGridUnit, &m_yGridUnit); }

private:
  constexpr static float k_defaultGridUnit = 2.f;

  void privateSet(float f, float limit, Poincare::Range1D & range1D, void (Poincare::Range1D::*setter)(float, float), bool updateGridUnit, float * gridUnit);

  // Window bounds of the data
  Poincare::Range2D m_range;
  float m_xGridUnit;
  float m_yGridUnit;
};

}

#endif
