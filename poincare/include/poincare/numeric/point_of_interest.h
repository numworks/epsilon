#ifndef POINCARE_NUMERIC_POINT_OF_INTEREST_H
#define POINCARE_NUMERIC_POINT_OF_INTEREST_H

#include <poincare/numeric/solver.h>

namespace Poincare {

struct PointOfInterest {
  double abscissa = NAN;
  double ordinate;
  uint32_t data;
  Solver<double>::Interest interest;
  bool inverted;
  uint8_t subCurveIndex;

  double x() const { return inverted ? ordinate : abscissa; }
  double y() const { return inverted ? abscissa : ordinate; }
  Coordinate2D<double> xy() const { return {x(), y()}; }
  bool isUninitialized() const { return std::isnan(abscissa); }
};

}  // namespace Poincare

#endif
