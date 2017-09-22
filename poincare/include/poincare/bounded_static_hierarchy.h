#ifndef POINCARE_BOUNDED_STATIC_HIERARCHY_H
#define POINCARE_BOUNDED_STATIC_HIERARCHY_H

#include <poincare/static_hierarchy.h>

namespace Poincare {

template<int T>
class BoundedStaticHierarchy : public StaticHierarchy<T> {
public:
  BoundedStaticHierarchy();
  BoundedStaticHierarchy(Expression * const * operands, int numberOfOperands, bool cloneOperands = true);
  int numberOfOperands() const override { return m_numberOfOperands; }
  bool hasValidNumberOfArguments() const override;
private:
  int m_numberOfOperands;
};

}

#endif
