#ifndef SHARED_MEMOIZED_CURVE_VIEW_RANGE_H
#define SHARED_MEMOIZED_CURVE_VIEW_RANGE_H

#include <poincare/range.h>

#include "curve_view_range.h"

namespace Shared {

class MemoizedCurveViewRange : public CurveViewRange {
 public:
  template <typename T>
  struct AxisInformation {
    T x, y;
    T operator()(Axis axis) const { return axis == Axis::X ? x : y; }
    void set(Axis axis, T value) { (axis == Axis::X ? x : y) = value; }
  };

  MemoizedCurveViewRange();

  // CurveViewRange
  float xMin() const override { return m_range.xMin(); }
  float xMax() const override { return m_range.xMax(); }
  float yMin() const override { return m_range.yMin(); }
  float yMax() const override { return m_range.yMax(); }
  /* A null gridUnit value means a limit has been changed without updating the
   * grid unit. */
  float xGridUnit() override final;
  float yGridUnit() override final;
  virtual void setXRange(float min, float max) { protectedSetXRange(min, max); }
  virtual void setYRange(float min, float max) { protectedSetYRange(min, max); }

 protected:
  Poincare::Range2D<float> memoizedRange() const { return m_range; }
  virtual float computeGridUnit(Axis axis) {
    return axis == Axis::X ? CurveViewRange::xGridUnit()
                           : CurveViewRange::yGridUnit();
  }

  void protectedSetXRange(float min, float max,
                          float limit = Poincare::Range1D<float>::k_maxFloat) {
    privateSet(min, max, limit, true);
  }
  void protectedSetYRange(float min, float max,
                          float limit = Poincare::Range1D<float>::k_maxFloat) {
    privateSet(min, max, limit, false);
  }

  void protectedSetXRange(Poincare::Range1D<float> range,
                          float limit = Poincare::Range1D<float>::k_maxFloat) {
    protectedSetXRange(range.min(), range.max(), limit);
  }
  void protectedSetYRange(Poincare::Range1D<float> range,
                          float limit = Poincare::Range1D<float>::k_maxFloat) {
    protectedSetYRange(range.min(), range.max(), limit);
  }

 private:
  void privateSet(float min, float max, float limit, bool x);

  // Window bounds of the data
  Poincare::Range2D<float> m_range;
  AxisInformation<float> m_gridUnit;
};

}  // namespace Shared

#endif
