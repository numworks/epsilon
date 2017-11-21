#ifndef POINCARE_SIMPLIFICATION_SELECTOR_TYPE_SELECTOR_H
#define POINCARE_SIMPLIFICATION_SELECTOR_TYPE_SELECTOR_H

#include "selector.h"

namespace Poincare {
namespace Simplification {

class TypeSelector : public Selector {
public:
  constexpr TypeSelector(Expression::Type type, int captureIndex = -1, const Selector * const * children = nullptr, int numberOfChildren = 0, int childrenPartialMatch = true) :
    Selector(captureIndex, children, numberOfChildren, childrenPartialMatch), m_type(type) {}
  bool immediateMatch(const Expression * e) const override;
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
private:
  Expression::Type m_type;
};

}
}

#endif

