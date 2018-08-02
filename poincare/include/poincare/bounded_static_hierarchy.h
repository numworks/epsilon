#ifndef POINCARE_BOUNDED_STATIC_HIERARCHY_H
#define POINCARE_BOUNDED_STATIC_HIERARCHY_H

#include <poincare/static_hierarchy.h>

namespace Poincare {

template<int T>
class BoundedStaticHierarchy : public StaticHierarchy<T> {
public:
  BoundedStaticHierarchy();
  BoundedStaticHierarchy(const Expression * expression, bool cloneOperands = true); // Specialized constructor for StaticHierarchy<2>
  BoundedStaticHierarchy(const Expression * expression1, const Expression * expression2, bool cloneOperands = true); // Specialized constructor for StaticHierarchy<2>
  BoundedStaticHierarchy(const Expression * const * operands, int numberOfChildren, bool cloneOperands = true);
  void setArgument(ListData * listData, int numberOfEntries, bool clone) override;
  int numberOfChildren() const override { return m_numberOfChildren; }
  bool hasValidNumberOfOperands(int numberOfChildren) const override;
private:
  int m_numberOfChildren;
};

}

#endif
