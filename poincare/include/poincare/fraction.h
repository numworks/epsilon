#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/binary_operation.h>

class Fraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    ExpressionLayout * createLayout() override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
};

#endif
