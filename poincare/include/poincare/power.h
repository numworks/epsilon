#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/float.h>
#include <poincare/matrix.h>

#include <poincare/binary_operation.h>

class Power : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    ExpressionLayout * createLayout() const override;
    Expression * createEvaluation(Context& context) const override;
    float approximate(Context& context) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
  private:
    Expression * createEvaluationOnMatrixAndInteger(Matrix * m, Integer * i, Context& context) const;
};

#endif
