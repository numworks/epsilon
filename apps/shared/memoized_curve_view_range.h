#ifndef SHARED_MEMOIZED_CURVE_VIEW_RANGE_H
#define SHARED_MEMOIZED_CURVE_VIEW_RANGE_H

#include "curve_view_range.h"
#include "range_1D.h"

namespace Shared {

class MemoizedCurveViewRange : public CurveViewRange {
public:
  MemoizedCurveViewRange();
  //CurveViewWindow
  float xMin() const override { return m_xRange.min(); }
  float xMax() const override { return m_xRange.max(); }
  float yMin() const override { return m_yRange.min(); }
  float yMax() const override { return m_yRange.max(); }
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
  void protectedSetXMin(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY, bool updateGridUnit = true);
  void protectedSetXMax(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY, bool updateGridUnit = true);
  void protectedSetYMin(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY, bool updateGridUnit = true);
  void protectedSetYMax(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY, bool updateGridUnit = true);
private:
  // Window bounds of the data
  Range1D m_xRange;
  Range1D m_yRange;
  float m_xGridUnit;
  float m_yGridUnit;
};

}

#endif
