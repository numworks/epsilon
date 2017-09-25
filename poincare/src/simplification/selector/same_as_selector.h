#ifndef POINCARE_SIMPLIFICATION_SAME_AS_SELECTOR_H
#define POINCARE_SIMPLIFICATION_SAME_AS_SELECTOR_H

#include "selector.h"

namespace Poincare {
namespace Simplification {

class SameAsSelector : public Selector {
public:
  constexpr SameAsSelector(int originalIndex, int captureIndex = -1, Selector ** children = nullptr, int numberOfChildren = 0, bool childrenPartialMatch = true) :
    Selector(captureIndex, children, numberOfChildren, childrenPartialMatch), m_originalIndex(originalIndex) {}
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
private:
  int m_originalIndex;
};

}
}

#endif
