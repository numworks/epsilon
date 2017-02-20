#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Subtraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context& context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode) const override;
  Expression * evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const override;
  Expression * evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const override;
};

}

#endif
