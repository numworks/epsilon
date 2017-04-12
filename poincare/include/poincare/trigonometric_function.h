#ifndef POINCARE_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_TRIGONOMETRIC_FUNCTION_H

#include <poincare/function.h>

namespace Poincare {

class TrigonometricFunction : public Function {
public:
  TrigonometricFunction(const char * name);
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
  virtual float trigonometricApproximation(float x) const = 0;
  virtual Expression * createComplexEvaluation(Expression * arg, Context & context, AngleUnit angleUnit) const = 0;
};

}

#endif
