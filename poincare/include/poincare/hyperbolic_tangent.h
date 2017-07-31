#ifndef POINCARE_HYPERBOLIC_TANGENT_H
#define POINCARE_HYPERBOLIC_TANGENT_H

#include <poincare/function.h>

namespace Poincare {

class HyperbolicTangent : public Function {
public:
  HyperbolicTangent();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  static Complex compute(const Complex c);
private:
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override {
    return compute(c);
  }
};

}

#endif
