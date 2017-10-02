#include "type_selector.h"
#include "combination.h"

namespace Poincare {
namespace Simplification {

bool TypeSelector::immediateMatch(const Expression * e) const {
  return (e->type() == m_type);
}

bool TypeSelector::acceptsLocationInCombination(const Combination * combination, int location) const {
  // Yes if all this is true:
  // - The Expression matches matches my Class
  // - The proposed location is strictly after the one for my previous brother, if I have one.
  //     -> This requires to SORT selectors, and put the "ANY"/"SameAs" at the end
  //   Code is stupid : m[locationInMatch] > m[locationInMatch-1];

  if (location > 0 && (combination->expressionIndexForSelectorIndex(location) <= combination->expressionIndexForSelectorIndex(location-1))) {
    return false;
  }

  return Selector::acceptsLocationInCombination(combination, location);
}

}
}
