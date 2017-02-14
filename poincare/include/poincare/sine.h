#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/function.h>

namespace Poincare {

class Sine : public Function {
public:
  Sine();
  float approximate(Context & context, AngleUnit angleUnit) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
};

}

#endif
