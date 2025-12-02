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

  /* Sort the interest points on their x coordinate if [alongX], or on their
   * abscissa otherwise. */
  void sort(bool alongX);
  // Filter out interest of abscissa not between [start] and [end]
  void filterOutOfBounds(double start, double end);

 private:
  Expression m_list;
};

}  // namespace Poincare
