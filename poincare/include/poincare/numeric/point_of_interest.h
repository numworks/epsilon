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
  PointsOfInterestList() : m_stackList(nullptr), m_poolList() {}

  void init() { m_poolList = API::JuniorPoolHandle::Builder(KList()); }
  bool isUninitialized() const { return m_poolList.isUninitialized(); }
  void moveToStack();
  void moveToPool();

  int numberOfPoints() const;
  PointOfInterest pointAtIndex(int) const;
  void sort();
  void filterOutOfBounds(double start, double end);
  void append(PointOfInterest);

 private:
  Internal::Tree* m_stackList;
  API::JuniorPoolHandle m_poolList;
};

}  // namespace Poincare

#endif
