#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/commutative_operation.h>

class Addition : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    ExpressionLayout * createLayout(ExpressionLayout * parent) override;
    Type type() override;
    float operateApproximatevelyOn(float a, float b) override;
    Expression * clone() override;
};

#endif
