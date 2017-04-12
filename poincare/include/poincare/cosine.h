#ifndef POINCARE_COSINE_H
#define POINCARE_COSINE_H

#include <poincare/trigonometric_function.h>

namespace Poincare {

class Cosine : public TrigonometricFunction {
public:
  Cosine();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  float trigonometricApproximation(float x) const override;
  Expression * createComplexEvaluation(Expression * arg, Context & context, AngleUnit angleUnit) const override;
};

}

#endif
