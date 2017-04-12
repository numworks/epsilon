#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/trigonometric_function.h>

namespace Poincare {

class Sine : public TrigonometricFunction {
public:
  Sine();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  float trigonometricApproximation(float x) const override;
  Expression * createComplexEvaluation(Expression * arg, Context & context, AngleUnit angleUnit) const override;
};

}

#endif
