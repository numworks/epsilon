#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Subtraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  ExpressionLayout * createLayout(FloatDisplayMode FloatDisplayMode = FloatDisplayMode::Auto) const override;
  float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  Expression * evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const override;
  Expression * evaluateOnComplexAndMatrix(Complex * c, Matrix * m, Context& context, AngleUnit angleUnit) const override;
};

}

#endif
