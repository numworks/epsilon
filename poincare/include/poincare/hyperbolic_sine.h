#ifndef POINCARE_HYPERBOLIC_SINE_H
#define POINCARE_HYPERBOLIC_SINE_H

#include <poincare/function.h>

namespace Poincare {

class HyperbolicSine : public Function {
public:
  HyperbolicSine();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  template<typename T> static Complex<T> compute(const Complex<T> c);
private:
  Complex<float> computeComplex(const Complex<float> c, AngleUnit angleUnit) const override {
    return compute(c);
  }
  Complex<double> computeComplex(const Complex<double> c, AngleUnit angleUnit) const override {
    return compute(c);
  }
};

}

#endif
