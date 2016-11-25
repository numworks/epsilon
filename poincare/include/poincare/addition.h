#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/commutative_operation.h>
#include <poincare/float.h>
#include <poincare/matrix.h>

class Addition : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    Type type() const override;
    float operateApproximatevelyOn(float a, float b) const override;
    Expression * createEvaluationOn(Expression * a, Expression * b, Context& context) const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const override;
  protected:
    char operatorChar() const override;
  private:
    Expression * createEvaluationOnFloatAndMatrix(Float * a, Matrix * m, Context& context) const;
    Expression * createEvaluationOnMatrices(Matrix * m, Matrix * n, Context& context) const;
};

#endif
