#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <cmath>
#include <poincare/function.h>
#include <poincare/variable_context.h>

using std::isinf;
using std::isnan;

namespace Poincare {

class Derivative : public Function {
public:
  Derivative();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  template<typename T> T growthRateAroundAbscissa(T x, T h, VariableContext<T> variableContext, AngleUnit angleUnit) const;
  template<typename T> T approximateDerivate2(T x, T h, VariableContext<T> xContext, AngleUnit angleUnit) const;
  // TODO: Change coefficients?
  constexpr static double k_maxErrorRateOnApproximation = 0.001;
  constexpr static double k_minInitialRate = 0.01;
  constexpr static double k_rateStepSize = 1.4;
};

}

#endif
