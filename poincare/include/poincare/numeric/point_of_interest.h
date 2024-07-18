#ifndef POINCARE_NUMERIC_POINT_OF_INTEREST_H
#define POINCARE_NUMERIC_POINT_OF_INTEREST_H

#include <poincare/api.h>
#include <poincare/k_tree.h>
#include <poincare/numeric/solver.h>

namespace Poincare {

struct PointOfInterest {
  double abscissa;
  double ordinate;
  uint32_t data;
  Internal::Solver<double>::Interest interest;
  bool inverted;
  uint8_t subCurveIndex;

  double x() const { return inverted ? ordinate : abscissa; }
  double y() const { return inverted ? abscissa : ordinate; }
  Coordinate2D<double> xy() const { return {x(), y()}; }
  bool isUninitialized() const { return std::isnan(abscissa); }
};

class PointsOfInterestList {
 public:
  void init() { m_list = API::JuniorPoolHandle::Builder(KList()); }
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const;
  PointOfInterest pointAtIndex(int) const;
  void sort();
  void filterOutOfBounds(double start, double end);
  void append(PointOfInterest) {}  // TODO

 private:
  API::JuniorPoolHandle m_list;
};

}  // namespace Poincare

#endif
