#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/function.h>

class Sum : public Function {
public:
  Sum();
  float approximate(Context & context, AngleUnit angleUnit) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
};

#endif
