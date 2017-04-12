#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/trigonometric_function.h>

namespace Poincare {

class Tangent : public TrigonometricFunction {
public:
  Tangent();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  float trigonometricApproximation(float x) const override;
  Expression * createComplexEvaluation(Expression * arg, Context & context, AngleUnit angleUnit) const override;
};

}

#endif
