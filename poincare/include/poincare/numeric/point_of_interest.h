#ifndef POINCARE_NUMERIC_POINT_OF_INTEREST_H
#define POINCARE_NUMERIC_POINT_OF_INTEREST_H

#include <poincare/api.h>
#include <poincare/numeric/solver.h>

namespace Poincare {

struct PointOfInterest {
  double abscissa = NAN;
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
  void init();
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const;
  PointOfInterest pointAtIndex(int) const;

  using Provider = PointOfInterest (*)(void*);
  static API::JuniorPoolHandle BuildStash(Provider, void* providerContext);
  /* Consume the argument, and steal its children. */
  bool merge(API::JuniorPoolHandle&);

  void sort();
  void filterOutOfBounds(double start, double end);

 private:
  API::JuniorPoolHandle m_list;
};

}  // namespace Poincare

#endif
