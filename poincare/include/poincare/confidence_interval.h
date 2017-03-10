#ifndef POINCARE_CONFIDENCE_INTERVAL_H
#define POINCARE_CONFIDENCE_INTERVAL_H

#include <poincare/function.h>

namespace Poincare {

class ConfidenceInterval : public Function {
public:
  ConfidenceInterval();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif

