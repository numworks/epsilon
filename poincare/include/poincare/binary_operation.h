#ifndef POINCARE_BINARY_OPERATION_H
#define POINCARE_BINARY_OPERATION_H

#include <poincare/expression.h>
#include <poincare/matrix.h>
#include <poincare/float.h>

class BinaryOperation : public Expression {
  public:
    BinaryOperation(Expression ** operands, bool cloneOperands = true);
    ~BinaryOperation();
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * clone() const override;
    Expression * evaluate(Context& context) const override;
  protected:
    Expression * m_operands[2];
    virtual Expression * evaluateOnMatrixAndFloat(Matrix * m, Float * f, Context& context) const;
    virtual Expression * evaluateOnFloatAndMatrix(Float * f, Matrix * m, Context& context) const;
    virtual Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context) const;
};

#endif
