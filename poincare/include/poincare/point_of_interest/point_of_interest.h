#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/numeric_solver/interest.h>
#include <stdint.h>

/* This struct as well as PointOfInterestList should probably be moved to
 * Epsilon folder. They are currently in Poincare only because we want to store
 * them in the stack. We should replace the PointOfInterest type with a
 * FixedSizeData type or similar. */

namespace Poincare {

struct PointOfInterest {
  double abscissa = NAN;
  double ordinate = NAN;
  uint32_t data = 0;
  Interest interest = Interest::None;
  bool inverted = false;
  /* Used for unreached points of interest (inequalities, unreached
   * discontinuities, intersections with strict inequalities) */
  bool isUnreached = false;
  uint8_t subCurveIndex = 0;

  double x() const { return inverted ? ordinate : abscissa; }
  double y() const { return inverted ? abscissa : ordinate; }
  Coordinate2D<double> xy() const { return {x(), y()}; }
  bool isInitialized() const { return !std::isnan(abscissa); }
};

}  // namespace Poincare
