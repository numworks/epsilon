#ifndef POINCARE_POWER_H
#define POINCARE_POWER_H

#include <poincare/binary_operation.h>

class Power : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
  public:
    ExpressionLayout * createLayout() override;
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
#ifdef DEBUG
    int getPrintableVersion(char* txt) override;
#endif
};

#endif
