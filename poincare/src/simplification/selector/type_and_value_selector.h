#ifndef POINCARE_SIMPLIFICATION_SELECTOR_TYPE_AND_VALUE_SELECTOR_H
#define POINCARE_SIMPLIFICATION_SELECTOR_TYPE_AND_VALUE_SELECTOR_H

#include "type_selector.h"

namespace Poincare {
namespace Simplification {

class TypeAndValueSelector : public TypeSelector {
public:
  constexpr TypeAndValueSelector(Expression::Type type, int value, int captureIndex = -1, Selector ** children = nullptr, int numberOfChildren = 0, bool childrenPartialMatch = true) : TypeSelector(type, captureIndex, children, numberOfChildren, childrenPartialMatch), m_value(value) { }
  bool immediateMatch(const Expression * e) const override;
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
private:
  int m_value;
};

}
}

#endif
