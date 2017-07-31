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
  static Complex compute(const Complex c);
private:
  Complex privateCompute(const Complex c, AngleUnit angleUnit) const override {
    return compute(c);
  }
  float computeForRadianReal(float x) const override;
};

}

#endif
