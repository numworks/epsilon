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
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif
