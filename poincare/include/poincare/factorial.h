#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include <cmath>
#include <poincare/function.h>

using std::isinf;
using std::isnan;

namespace Poincare {

class Factorial : public Function {
public:
  Factorial(Expression * argument, bool clone = true);
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Complex<float> computeComplex(const Complex<float> c, AngleUnit angleUnit) const override {
    return templatedComputeComplex(c);
  }
  Complex<double> computeComplex(const Complex<double> c, AngleUnit angleUnit) const override {
    return templatedComputeComplex(c);
  }
  template<typename T> Complex<T> templatedComputeComplex(const Complex<T> c) const;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif
