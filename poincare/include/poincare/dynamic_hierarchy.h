#ifndef POINCARE_DYNAMIC_HIERARCHY_H
#define POINCARE_DYNAMIC_HIERARCHY_H

#include <poincare/expression.h>
#include <poincare/expression_array.h>
#include <poincare/rational.h>

namespace Poincare {

class DynamicHierarchy : public Expression {
public:
  DynamicHierarchy();
  DynamicHierarchy(const Expression * const * operands, int numberOfChildren, bool cloneOperands);
  DynamicHierarchy(const Expression * operand1, const Expression * operand2, bool cloneOperands) :
    DynamicHierarchy(ExpressionArray(operand1, operand2).array(), 2, cloneOperands) {}
  ~DynamicHierarchy();
  DynamicHierarchy(const DynamicHierarchy & other) = delete;
  DynamicHierarchy(DynamicHierarchy && other) = delete;
  DynamicHierarchy& operator=(const DynamicHierarchy & other) = delete;
  DynamicHierarchy& operator=(DynamicHierarchy && other) = delete;

  int numberOfChildren() const override { return m_numberOfChildren; }
  const Expression * const * operands() const override { return m_operands; };

  void removeOperand(const Expression * e, bool deleteAfterRemoval = true);
  void addOperands(const Expression * const * operands, int numberOfChildren);
  void addOperand(Expression * operand);
  void addOperandAtIndex(Expression * operand, int index);
  void mergeOperands(DynamicHierarchy * d);
  typedef int (*ExpressionOrder)(const Expression * e1, const Expression * e2, bool canBeInterrupted);
  void sortOperands(ExpressionOrder order, bool canBeInterrupted);
  Expression * squashUnaryHierarchy();
protected:
  const Expression ** m_operands;
  int m_numberOfChildren;
private:
  void removeOperandAtIndex(int i, bool deleteAfterRemoval);
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override;
  int simplificationOrderGreaterType(const Expression * e, bool canBeInterrupted) const override;
};

}

#endif
