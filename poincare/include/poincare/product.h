#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/commutative_operation.h>

class Product : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    Type type() override;
    float operateApproximatevelyOn(float a, float b) override;
};

#endif
