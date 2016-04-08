#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/binary_operation.h>

class Power : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    ExpressionLayout * createLayout() override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) override;
};

#endif
