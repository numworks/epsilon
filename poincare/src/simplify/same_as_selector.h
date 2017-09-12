#ifndef POINCARE_SIMPLIFY_SAME_AS_SELECTOR_H
#define POINCARE_SIMPLIFY_SAME_AS_SELECTOR_H

#include "selector.h"

 namespace Poincare {

class SameAsSelector : public Selector {
public:
  constexpr SameAsSelector(int originalIndex, int captureIndex = -1, Selector ** children = nullptr, int numberOfChildren = 0);
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
private:
  int m_originalIndex;
};

constexpr SameAsSelector::SameAsSelector(int originalIndex, int captureIndex, Selector ** children, int numberOfChildren) :
  Selector(captureIndex, children, numberOfChildren),
  m_originalIndex(originalIndex)
{
}

}

#endif
