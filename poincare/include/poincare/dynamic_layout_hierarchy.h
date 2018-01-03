#ifndef POINCARE_DYNAMIC_LAYOUT_HIERARCHY_H
#define POINCARE_DYNAMIC_LAYOUT_HIERARCHY_H

#include <poincare/expression_layout.h>
#include <poincare/expression_layout_array.h>

namespace Poincare {

class DynamicLayoutHierarchy : public ExpressionLayout {
public:
  DynamicLayoutHierarchy();
  DynamicLayoutHierarchy(const ExpressionLayout * const * operands, int numberOfOperands, bool cloneOperands = true);
  DynamicLayoutHierarchy(const ExpressionLayout * operand1, const ExpressionLayout * operand2, bool cloneOperands = true) :
    DynamicLayoutHierarchy(ExpressionLayoutArray(operand1, operand2).array(), 2, cloneOperands) {}
  ~DynamicLayoutHierarchy();
  DynamicLayoutHierarchy(const DynamicLayoutHierarchy & other) = delete;
  DynamicLayoutHierarchy(DynamicLayoutHierarchy && other) = delete;
  DynamicLayoutHierarchy& operator=(const DynamicLayoutHierarchy & other) = delete;
  DynamicLayoutHierarchy& operator=(DynamicLayoutHierarchy && other) = delete;

  int numberOfChildren() const override { return m_numberOfChildren; }
  const ExpressionLayout * const * children() const override { return m_children; };

  bool addChildAtIndex(ExpressionLayout * operand, int index) override;
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;
protected:
  const ExpressionLayout ** m_children;
  int m_numberOfChildren;
};

}

#endif
