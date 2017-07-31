#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/trigonometric_function.h>

namespace Poincare {

class Tangent : public TrigonometricFunction {
public:
  Tangent();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  Complex privateCompute(const Complex c, AngleUnit angleUnit) const override;
  float computeForRadianReal(float x) const override;
};

}

#endif
