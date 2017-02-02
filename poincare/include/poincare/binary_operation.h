#ifndef POINCARE_BINARY_OPERATION_H
#define POINCARE_BINARY_OPERATION_H

#include <poincare/expression.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>

class BinaryOperation : public Expression {
public:
  BinaryOperation(Expression ** operands, bool cloneOperands = true);
  ~BinaryOperation();
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
protected:
  Expression * m_operands[2];
  virtual Expression * evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const = 0;
  virtual Expression * evaluateOnMatrixAndComplex(Matrix * m, Complex * c, Context& context, AngleUnit angleUnit) const;
  virtual Expression * evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const;
  virtual Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const;
};

#endif
