#ifndef SHARED_CURVE_VIEW_RANGE_H
#define SHARED_CURVE_VIEW_RANGE_H

#include <stdint.h>

namespace Shared {

class CurveViewRange {
public:
  enum class Axis {
    X,
    Y
  };
  virtual uint32_t rangeChecksum();

  virtual float xMin() const = 0;
  virtual float xMax() const = 0;
  virtual float yMin() const = 0;
  virtual float yMax() const = 0;
  float xCenter() const { return (xMin() + xMax()) / 2; }
  float yCenter() const { return (yMin() + yMax()) / 2; }
  virtual float xGridUnit() const {
    return computeGridUnit(k_minNumberOfXGridUnits, k_maxNumberOfXGridUnits, xMax() - xMin());
  }
  virtual float yGridUnit() const {
    return computeGridUnit(k_minNumberOfYGridUnits, k_maxNumberOfYGridUnits, yMax() - yMin());
  }
  constexpr static float k_maxNumberOfXGridUnits = 18.0f;
  constexpr static float k_maxNumberOfYGridUnits = 13.0f;
private:
  constexpr static float k_minNumberOfXGridUnits = 7.0f;
  constexpr static float k_minNumberOfYGridUnits = 5.0f;
  /* The grid units is constrained to be a number of type: k*10^n with k = 1,2 or 5
   * and n a relative integer. The choice of x and y grid units depend on the
   * grid range.*/
  constexpr static float k_smallGridUnitMantissa = 1.0f;
  constexpr static float k_mediumGridUnitMantissa = 2.0f;
  constexpr static float k_largeGridUnitMantissa = 5.0f;
  float computeGridUnit(float minNumberOfUnits, float maxNumberOfUnits, float range) const;
};

}

#endif
