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
  // Intersection involving at least one strict inequation
  UnreachedIntersection,
  YIntercept,
};

struct PointOfInterest {
  double abscissa = NAN;
  double ordinate;
  uint32_t data;
  Interest interest;
  bool inverted;
  uint8_t subCurveIndex;

  double x() const { return inverted ? ordinate : abscissa; }
  double y() const { return inverted ? abscissa : ordinate; }
  Coordinate2D<double> xy() const { return {x(), y()}; }
  bool isUninitialized() const { return std::isnan(abscissa); }
};

}  // namespace Poincare
