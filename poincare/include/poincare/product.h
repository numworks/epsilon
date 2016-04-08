#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/commutative_operation.h>

class Product : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    Type type() override;
    float operateApproximatevelyOn(float a, float b) override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) override;
  protected:
    char operatorChar() override;
};

#endif
