#ifndef POINCARE_DYNAMIC_HIERARCHY_H
#define POINCARE_DYNAMIC_HIERARCHY_H

#include <poincare/hierarchy.h>

namespace Poincare {

class DynamicHierarchy : public Hierarchy {
public:
  DynamicHierarchy();
  DynamicHierarchy(Expression ** operands, int numberOfOperands, bool cloneOperands = true);
  ~DynamicHierarchy();
  DynamicHierarchy(const DynamicHierarchy& other) = delete;
  DynamicHierarchy(DynamicHierarchy&& other) = delete;
  DynamicHierarchy& operator=(const DynamicHierarchy& other) = delete;
  DynamicHierarchy& operator=(DynamicHierarchy&& other) = delete;
  int numberOfOperands() const override;
  const Expression * operand(int i) const override;
protected:
  Expression ** operands() override;
  Expression ** m_operands;
};

}

#endif
