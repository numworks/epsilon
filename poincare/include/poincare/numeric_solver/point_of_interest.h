#pragma once

#include <poincare/coordinate_2D.h>
#include <stdint.h>

namespace Poincare {

enum class Interest : uint8_t {
  None,
  Root,
  Minimum,
  Maximum,
  Discontinuity,
  ReachedDiscontinuity,    // Discontinuity where f(x) is defined
  UnreachedDiscontinuity,  // Discontinuity where f(x) is undefined
  Intersection,
  YIntercept,
};

struct PointOfInterest {
  double abscissa = NAN;
  double ordinate = NAN;
  uint32_t data = 0;
  Interest interest = Interest::None;
  bool inverted = false;
  // Used for unreached intersections
  // TODO: Also implement for unreached discontinuities
  bool isUnreached = false;
  uint8_t subCurveIndex = 0;

  double x() const { return inverted ? ordinate : abscissa; }
  double y() const { return inverted ? abscissa : ordinate; }
  Coordinate2D<double> xy() const { return {x(), y()}; }
  bool isInitialized() const { return !std::isnan(abscissa); }
};

}  // namespace Poincare
