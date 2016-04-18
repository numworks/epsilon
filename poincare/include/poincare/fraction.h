#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/binary_operation.h>

class Fraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    ExpressionLayout * createLayout() const override;
    float approximate(Context& context) const override;
    Type type() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numnerOfOperands, bool cloneOperands = true) const override;
};

#endif
