#ifndef POINCARE_CEILING_H
#define POINCARE_CEILING_H

#include <poincare/function.h>

namespace Poincare {

class Ceiling : public Function {
public:
  Ceiling();
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


