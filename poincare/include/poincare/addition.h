#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/commutative_operation.h>

class Addition : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    Type type() const override;
    float operateApproximatevelyOn(float a, float b) const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const override;
  protected:
    char operatorChar() const override;
};

#endif
