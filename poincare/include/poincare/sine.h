#ifndef POINCARE_SINE_H
#define POINCARE_SINE_H

#include <poincare/function.h>

namespace Poincare {

class Sine : public Function {
public:
  Sine();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, AngleUnit angleUnit = AngleUnit::Radian);
private:
  Complex<float> computeComplex(const Complex<float> c, AngleUnit angleUnit) const override {
    return compute(c, angleUnit);
  }
  Complex<double> computeComplex(const Complex<double> c, AngleUnit angleUnit) const override {
    return compute(c, angleUnit);
  }
};

}

#endif
