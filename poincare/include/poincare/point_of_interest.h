#ifndef POINCARE_POINT_OF_INTEREST_H
#define POINCARE_POINT_OF_INTEREST_H

#include <poincare/expression.h>
#include <poincare/solver.h>

namespace Poincare {

class PointOfInterest final : public JuniorExpression {
 public:
  static PointOfInterest Builder(double abscissa, double ordinate,
                                 typename Solver<double>::Interest interest,
                                 uint32_t data, bool inverted,
                                 int subCurveIndex);

  /* Abscissa/ordinate are from the function perspective, while x/y are related
   * to the drawings. They differ only with functions along y. */
  double abscissa() const;
  double ordinate() const;
  double x() const;
  double y() const;
  int subCurveIndex() const;
  typename Solver<double>::Interest interest() const;
  Coordinate2D<double> xy() const;
  uint32_t data() const;
};

class PointsOfInterestList {
 public:
  List list() const { return m_list; }
  void init() { m_list = List::Builder(); }
  void setList(List list) { m_list = list; }
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const { return m_list.numberOfChildren(); }
  Poincare::PointOfInterest pointAtIndex(int i) const;
  void append(double abscissa, double ordinate, uint32_t data,
              typename Solver<double>::Interest interest, bool inverted,
              int subCurveIndex);
  void sort();

 private:
  List m_list;
};

}  // namespace Poincare

#endif
