#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/float.h>
#include <poincare/matrix.h>
#include <poincare/binary_operation.h>


class Subtraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Expression * createEvaluation(Context& context) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
  private:
    Expression * createEvaluationOnMatrixAndFloat(Matrix * m, Float * f, Context& context, bool matrixMinusFloat) const;
    Expression * createEvaluationOnMatrices(Matrix * m, Matrix * n, Context& context) const;
};

#endif
