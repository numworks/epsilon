#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/function.h>

namespace Poincare {

class AbsoluteValue : public Function {
public:
  AbsoluteValue();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout(FloatDisplayMode FloatDisplayMode = FloatDisplayMode::Auto) const override;
};

}

#endif
