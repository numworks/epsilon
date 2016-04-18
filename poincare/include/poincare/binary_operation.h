#ifndef POINCARE_BINARY_OPERATION_H
#define POINCARE_BINARY_OPERATION_H

#include <poincare/expression.h>

class BinaryOperation : public Expression {
  public:
    BinaryOperation(Expression ** operands, bool cloneOperands = true);
    ~BinaryOperation();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
  protected:
    Expression * m_operands[2];
};

#endif
