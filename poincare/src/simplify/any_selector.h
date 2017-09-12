#ifndef POINCARE_SIMPLIFY_ANY_SELECTOR_H
#define POINCARE_SIMPLIFY_ANY_SELECTOR_H

#include "selector.h"

namespace Poincare {

class AnySelector : public Selector {
public:
  using Selector;
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
};

}

#endif
