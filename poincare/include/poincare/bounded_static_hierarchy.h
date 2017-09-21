#ifndef POINCARE_BOUNDED_STATIC_HIERARCHY_H
#define POINCARE_BOUNDED_STATIC_HIERARCHY_H

#include <poincare/static_hierarchy.h>

namespace Poincare {

template<int T>
class BoundedStaticHierarchy : public StaticHierarchy<T> {
public:
  BoundedStaticHierarchy();
  BoundedStaticHierarchy(Expression * const * operands, int numberOfOperands, bool cloneOperands = true);
  bool hasValidNumberOfArguments() const override;
};

}

#endif
