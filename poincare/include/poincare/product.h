#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/commutative_operation.h>

class Product : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    Type type() const override;
    float operateApproximatevelyOn(float a, float b) const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
  protected:
    char operatorChar() const override;
};

#endif
