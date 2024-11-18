#ifndef SHARED_CURVE_VIEW_RANGE_H
#define SHARED_CURVE_VIEW_RANGE_H

#include <omg/enums.h>
#include <poincare/numeric/zoom.h>
#include <stdint.h>

namespace Shared {

class CurveViewRange {
 public:
  uint32_t rangeChecksum();

  virtual float xMin() const = 0;
  virtual float xMax() const = 0;
  virtual float yMin() const = 0;
  virtual float yMax() const = 0;
  float xCenter() const { return (xMin() + xMax()) / 2; }
  float yCenter() const { return (yMin() + yMax()) / 2; }
  virtual float xGridUnit() { return computeGridUnit(OMG::Axis::Horizontal); }
  virtual float yGridUnit() { return computeGridUnit(OMG::Axis::Vertical); }
  constexpr static float k_maxNumberOfXGridUnits = 18.0f;
  constexpr static float k_maxNumberOfYGridUnits = 13.0f;

 protected:
  constexpr static float k_minNumberOfXGridUnits = 7.0f;
  constexpr static float k_minNumberOfYGridUnits = 5.0f;

 private:
  virtual float offscreenYAxis() const { return 0.f; }
  /* The grid units is constrained to be a number of type: k*10^n with k = 1,2
   * or 5 and n a relative integer. The choice of x and y grid units depend on
   * the grid range.*/
  constexpr static float k_smallGridUnitMantissa = 1.f;
  constexpr static float k_mediumGridUnitMantissa = 2.f;
  constexpr static float k_largeGridUnitMantissa = 5.f;
  float computeGridUnit(OMG::Axis axis);
};

}  // namespace Shared

#endif
