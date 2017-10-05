#ifndef POINCARE_DIVISION_QUOTIENT_H
#define POINCARE_DIVISION_QUOTIENT_H

#include <cmath>
#include <poincare/function.h>

using std::isinf;
using std::isnan;

namespace Poincare {

class DivisionQuotient : public Function {
public:
  DivisionQuotient();
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

