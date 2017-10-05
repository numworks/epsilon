#ifndef POINCARE_BINOMIAL_COEFFICIENT_H
#define POINCARE_BINOMIAL_COEFFICIENT_H

#include <poincare/function.h>
#include <cmath>

using std::isinf;
using std::isnan;

namespace Poincare {

class BinomialCoefficient : public Function {
public:
  BinomialCoefficient();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
};

}

#endif

