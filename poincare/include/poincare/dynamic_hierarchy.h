#ifndef POINCARE_DYNAMIC_HIERARCHY_H
#define POINCARE_DYNAMIC_HIERARCHY_H

#include <poincare/hierarchy.h>

namespace Poincare {

class DynamicHierarchy : public Hierarchy {
public:
  DynamicHierarchy();
  DynamicHierarchy(Expression * const * operands, int numberOfOperands, bool cloneOperands = true);
  ~DynamicHierarchy();
  DynamicHierarchy(const DynamicHierarchy & other) = delete;
  DynamicHierarchy(DynamicHierarchy && other) = delete;
  DynamicHierarchy& operator=(const DynamicHierarchy & other) = delete;
  DynamicHierarchy& operator=(DynamicHierarchy && other) = delete;

  int numberOfOperands() const override { return m_numberOfOperands; }
  Expression * const * operands() const override { return m_operands; };

  void removeOperand(const Expression * e, bool deleteAfterRemoval = true);
  void addOperands(Expression * const * operands, int numberOfOperands);
private:
  Expression ** m_operands;
  int m_numberOfOperands;
};

}

#endif
