#ifndef POINCARE_SQUARE_ROOT_H
#define POINCARE_SQUARE_ROOT_H

#include <poincare/function.h>

namespace Poincare {

class SquareRoot : public Function {
public:
  SquareRoot();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
};

}

#endif
