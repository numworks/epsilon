#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/binary_operation.h>
#include <poincare/float.h>
#include <poincare/matrix.h>

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
    Expression * evaluateOnMatrixAndFloat(Matrix * m, Float * f, Context& context, AngleUnit angleUnit) const override;
    Expression * evaluateOnFloatAndMatrix(Float * f, Matrix * m, Context& context, AngleUnit angleUnit) const override;
    Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context, AngleUnit angleUnit) const override;
};

#endif
