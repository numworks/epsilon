#ifndef POINCARE_COMMUTATIVE_OPERATION_H
#define POINCARE_COMMUTATIVE_OPERATION_H

#include <poincare/expression.h>

class CommutativeOperation : public Expression {
  public:
    CommutativeOperation(Expression ** operands, int numberOfOperands, bool cloneOperands = true);
    ~CommutativeOperation();
    Expression * operand(int i) override;
    int numberOfOperands() override;
    float approximate(Context& context) override;
    ExpressionLayout * createLayout() override;
    bool isCommutative() override;
  protected:
    virtual float operateApproximatevelyOn(float a, float b) = 0;
    virtual char operatorChar() = 0;
    int m_numberOfOperands;
    Expression ** m_operands;
};

#endif
