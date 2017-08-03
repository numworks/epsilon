#ifndef POINCARE_GREAT_COMMON_DIVISOR_H
#define POINCARE_GREAT_COMMON_DIVISOR_H

#include <poincare/function.h>

namespace Poincare {

class GreatCommonDivisor : public Function {
public:
  GreatCommonDivisor();
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

