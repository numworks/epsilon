#pragma once

#include <poincare/expression.h>

#include "point_of_interest.h"

namespace Poincare {

class PointsOfInterestList {
 public:
  void init();
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const;
  PointOfInterest pointAtIndex(int) const;

  using Provider = PointOfInterest (*)(void*);
  static Expression BuildStash(Provider, void* providerContext);
  /* Consume the argument, and steal its children. */
  bool merge(Expression&);

  // Sort the interest points on their x coordinate
  void sortX();
  // Sort the interest points on their abscissa
  void sortAbscissa();
  // Filter out interest of abscissa not between [start] and [end]
  void filterOutOfBounds(double start, double end);

 private:
  Expression m_list;
};

}  // namespace Poincare
