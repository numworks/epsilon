#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/binary_operation.h>
#include <poincare/float.h>
#include <poincare/matrix.h>

class Product : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    Type type() const override;
    ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
  private:
    Expression * evaluateOnMatrixAndFloat(Matrix * m, Float * f, Context& context) const override;
    Expression * evaluateOnFloatAndMatrix(Float * f, Matrix * m, Context& context) const override;
    Expression * evaluateOnMatrices(Matrix * m, Matrix * n, Context& context) const override;
};

#endif
