#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/function.h>

class AbsoluteValue : public Function {
public:
  AbsoluteValue();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
};

#endif
