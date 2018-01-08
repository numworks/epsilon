#ifndef POINCARE_BOUNDED_STATIC_LAYOUT_HIERARCHY_H
#define POINCARE_BOUNDED_STATIC_LAYOUT_HIERARCHY_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

template<int T>
class BoundedStaticLayoutHierarchy : public StaticLayoutHierarchy<T> {
public:
  BoundedStaticLayoutHierarchy();
  BoundedStaticLayoutHierarchy(const ExpressionLayout * expressionLayout, bool cloneOperands = true); // Specialized constructor for StaticLayoutHierarchy<2>
  BoundedStaticLayoutHierarchy(const ExpressionLayout * expressionLayout1, const ExpressionLayout * expressionLayout2, bool cloneOperands = true); // Specialized constructor for StaticLayoutHierarchy<2>
  BoundedStaticLayoutHierarchy(const ExpressionLayout * const * operands, int numberOfOperands, bool cloneOperands = true);
  int numberOfChildren() const override { return m_numberOfChildren; }
private:
  int m_numberOfChildren;
};

}

#endif
