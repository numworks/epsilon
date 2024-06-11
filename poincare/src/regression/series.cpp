#include "series.h"

namespace Poincare::Regression {

bool Series::numberOfDistinctAbscissaeGreaterOrEqualTo(int i) const {
  int count = 0;
  for (int j = 0; j < numberOfPairs(); j++) {
    if (count >= i) {
      return true;
    }
    double currentAbscissa = getX(j);
    bool firstOccurrence = true;
    for (int k = 0; k < j; k++) {
      if (getX(k) == currentAbscissa) {
        firstOccurrence = false;
        break;
      }
    }
    if (firstOccurrence) {
      count++;
    }
  }
  return count >= i;
}

}  // namespace Poincare::Regression
