#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/binary_operation.h>
#include <poincare/float.h>
#include <poincare/matrix.h>

class Fraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
  float approximate(Context& context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  Expression * evaluateOnMatrixAndFloat(Matrix * m, Float * f, Context& context) const override;
  Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context) const override;
};

#endif
