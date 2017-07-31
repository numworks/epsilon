#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/function.h>

namespace Poincare {

class Logarithm : public Function {
public:
  Logarithm();
  bool hasValidNumberOfArguments() const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context & context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
};

}

#endif
