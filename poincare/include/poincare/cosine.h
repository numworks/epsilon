#ifndef POINCARE_COSINE_H
#define POINCARE_COSINE_H

#include <poincare/trigonometric_function.h>

namespace Poincare {

class Cosine : public TrigonometricFunction {
public:
  Cosine();
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
