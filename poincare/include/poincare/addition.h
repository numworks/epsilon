#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/binary_operation.h>

class Addition : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
  float approximate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
      int numnerOfOperands, bool cloneOperands = true) const override;
  bool isCommutative() const override;
private:
  Expression * evaluateOnComplex(Complex * c, Complex * d, Context& context, AngleUnit angleUnit) const override;
};

#endif
