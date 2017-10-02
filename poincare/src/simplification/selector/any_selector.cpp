#include "any_selector.h"
#include "combination.h"

namespace Poincare {
namespace Simplification {

bool AnySelector::acceptsLocationInCombination(const Combination * combination, int location) const {
  // Yes, if no other slot in the combination before me is the same
  for (int i=0; i<location; i++) {
    if (combination->expressionIndexForSelectorIndex(i) == combination->expressionIndexForSelectorIndex(location)) {
      return false;
    }
  }
  return immediateMatch(combination->expressionForSelectorIndex(location));
}

}
}
