#ifndef POINCARE_COMMUTATIVE_OPERATION_H
#define POINCARE_COMMUTATIVE_OPERATION_H

#include <poincare/expression.h>

class CommutativeOperation : public Expression {
  public:
    CommutativeOperation(Expression ** operands, int numberOfOperands, bool cloneOperands = true);
    ~CommutativeOperation();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    float approximate(Context& context) const override;
    ExpressionLayout * createLayout() const override;
    bool isCommutative() const override;
    Expression * clone() const override;
  protected:
    virtual float operateApproximatevelyOn(float a, float b) const = 0;
    virtual char operatorChar() const = 0;
    const int m_numberOfOperands;
    Expression ** m_operands;
};

#endif
