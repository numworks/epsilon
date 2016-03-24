#ifndef POINCARE_BINARY_OPERATION_H
#define POINCARE_BINARY_OPERATION_H

#include <poincare/expression.h>

class BinaryOperation : public Expression {
  public:
    BinaryOperation(Expression ** operands, bool cloneOperands = true);
    ~BinaryOperation();
    Expression * operand(int i) override;
    int numberOfOperands() override;
  protected:
    Expression * m_operands[2];
};

#endif
