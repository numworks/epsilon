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
  BoundedStaticHierarchy(const Expression * const * operands, int numberOfOperands, bool cloneOperands = true);
  void setArgument(ListData * listData, int numberOfEntries, bool clone) override;
  int numberOfOperands() const override { return m_numberOfOperands; }
  bool hasValidNumberOfOperands(int numberOfOperands) const override;
private:
  int m_numberOfOperands;
};

}

#endif
