#ifndef POINCARE_SQUARE_ROOT_H
#define POINCARE_SQUARE_ROOT_H

#include <poincare/function.h>

class SquareRoot : public Function {
public:
  SquareRoot();
  float approximate(Context & context, AngleUnit angleUnit) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout(DisplayMode displayMode = DisplayMode::Auto) const override;
};

#endif
