#ifndef POINCARE_SIMPLIFY_TYPE_AND_VALUE_SELECTOR_H
#define POINCARE_SIMPLIFY_TYPE_AND_VALUE_SELECTOR_H

#include "selector.h"

 namespace Poincare {

class TypeAndValueSelector : public Selector {
public:
  constexpr TypeAndValueSelector(Expression::Type type, int value, int captureIndex = -1, Selector ** children = nullptr, int numberOfChildren = 0);
  bool acceptsLocationInCombination(const Combination * combination, int location) const override;
private:
  Expression::Type m_type;
  int m_value;
};

constexpr TypeAndValueSelector::TypeAndValueSelector(Expression::Type type, int value, int captureIndex, Selector ** children, int numberOfChildren) :
  Selector(captureIndex, children, numberOfChildren),
  m_type(type),
  m_value(value)
{
}

}

#endif
