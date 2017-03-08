#ifndef POINCARE_ABSOLUTE_VALUE_H
#define POINCARE_ABSOLUTE_VALUE_H

#include <poincare/function.h>

namespace Poincare {

class AbsoluteValue : public Function {
public:
  AbsoluteValue();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif
