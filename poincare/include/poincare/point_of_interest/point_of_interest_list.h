#pragma once

#include <poincare/expression.h>

#include "point_of_interest.h"

/* This class as well as PointOfInterest should probably be moved to
 * Epsilon folder. They are currently in Poincare only because we want to store
 * them in the stack. We should replace the PointOfInterestList class with a
 * templated List class that would encompass this as well as FloatList */

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
