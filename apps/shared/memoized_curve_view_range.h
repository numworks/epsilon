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
    T operator()(OMG::Axis axis) const {
      return axis == OMG::Axis::Horizontal ? x : y;
    }
    void set(OMG::Axis axis, T value) {
      (axis == OMG::Axis::Horizontal ? x : y) = value;
    }
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

  // TODO: create setters for double float numbers
  virtual void setXRange(float min, float max) { protectedSetXRange(min, max); }
  virtual void setYRange(float min, float max) { protectedSetYRange(min, max); }

  virtual void setXRange(Poincare::Range1D<float> range) {
    setXRange(range.min(), range.max());
  }
  virtual void setYRange(Poincare::Range1D<float> range) {
    setYRange(range.min(), range.max());
  }

 protected:
  Poincare::Range2D<float> memoizedRange() const { return m_range; }
  virtual float computeGridUnit(OMG::Axis axis) {
    return axis == OMG::Axis::Horizontal ? CurveViewRange::xGridUnit()
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
