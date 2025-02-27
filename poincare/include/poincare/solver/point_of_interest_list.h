#ifndef POINCARE_NUMERIC_POINT_OF_INTEREST_LIST_H
#define POINCARE_NUMERIC_POINT_OF_INTEREST_LIST_H

#include <poincare/api.h>

#include "point_of_interest.h"

namespace Poincare {

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
