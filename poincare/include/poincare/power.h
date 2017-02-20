#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Power : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode) const override;
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  Expression * evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const override;
  Expression * evaluateOnMatrixAndComplex(Matrix * m, Complex * c, Context& context, AngleUnit angleUnit) const override;
  Expression * evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const override;
  Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const override;
};

}

#endif
