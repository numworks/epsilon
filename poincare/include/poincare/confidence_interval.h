#ifndef POINCARE_CONFIDENCE_INTERVAL_H
#define POINCARE_CONFIDENCE_INTERVAL_H

#include <poincare/function.h>
#include <cmath>

using std::isinf;
using std::isnan;

namespace Poincare {

class ConfidenceInterval : public Function {
public:
  ConfidenceInterval();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
};

}

#endif

