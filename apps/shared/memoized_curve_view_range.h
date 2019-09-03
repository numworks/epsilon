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
  float xGridUnit() const override { return m_xGridUnit; }
  float yGridUnit() const override { return m_yGridUnit; }
  virtual void setXMin(float f) { protectedSetXMin(f); }
  virtual void setXMax(float f) { protectedSetXMax(f); }
  virtual void setYMin(float f) { protectedSetYMin(f); }
  virtual void setYMax(float f) { protectedSetYMax(f); }
protected:
  void protectedSetXMin(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  void protectedSetXMax(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  void protectedSetYMin(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  void protectedSetYMax(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  // Window bounds of the data
  Range1D m_xRange;
  Range1D m_yRange;
  float m_xGridUnit;
  float m_yGridUnit;
};

}

#endif
