#ifndef POINCARE_SIMPLIFICATION_SELECTOR_TYPE_AND_VALUE_SELECTOR_H
#define POINCARE_SIMPLIFICATION_SELECTOR_TYPE_AND_VALUE_SELECTOR_H

#include "selector.h"

namespace Poincare {
namespace Simplification {

class TypeAndValueSelector : public Selector {
public:
  constexpr TypeAndValueSelector(Expression::Type type, int value, int captureIndex = -1, Selector ** children = nullptr, int numberOfChildren = 0) : Selector(captureIndex, children, numberOfChildren), m_type(type), m_value(value) { }
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
private:
  Expression::Type m_type;
  int m_value;
};

}
}

#endif
