#ifndef POINCARE_ARC_SINE_H
#define POINCARE_ARC_SINE_H

#include <poincare/function.h>

namespace Poincare {

class ArcSine : public Function {
public:
  ArcSine();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Complex<float> computeComplex(const Complex<float> c, AngleUnit angleUnit) const override {
    return templatedComputeComplex(c, angleUnit);
  }
  Complex<double> computeComplex(const Complex<double> c, AngleUnit angleUnit) const override {
    return templatedComputeComplex(c, angleUnit);
  }
  template<typename T> Complex<T> templatedComputeComplex(const Complex<T> c, AngleUnit angleUnit) const;
};

}

#endif
