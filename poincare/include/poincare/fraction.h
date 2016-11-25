#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/binary_operation.h>
#include <poincare/float.h>
#include <poincare/matrix.h>

class Fraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  ExpressionLayout * createLayout() const override;
  float approximate(Context& context) const override;
  Expression * createEvaluation(Context& context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  Expression * createEvaluationOnMatrixAndFloat(Matrix * m, Float * f, Context& context) const;
  Expression * createEvaluationOnMatrices(Matrix * m, Matrix * n, Context& context) const;
};

#endif
