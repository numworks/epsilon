#ifndef POINCARE_CONJUGATE_H
#define POINCARE_CONJUGATE_H

#include <poincare/function.h>

namespace Poincare {

class Conjugate : public Function {
public:
  Conjugate();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif

