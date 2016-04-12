#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/commutative_operation.h>

class Addition : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    Type type() override;
    float operateApproximatevelyOn(float a, float b) override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) override;
  protected:
    char operatorChar() override;
};

#endif
