#include <poincare/sequence.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

ExpressionLayout * Sequence::createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return createSequenceLayoutWithArgumentLayouts(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), operand(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), operand(2)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

template<typename T>
Evaluation<T> * Sequence::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * aInput = operand(1)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * bInput = operand(2)->privateApproximate(T(), context, angleUnit);
  T start = aInput->toScalar();
  T end = bInput->toScalar();
  delete aInput;
  delete bInput;
  if (std::isnan(start) || std::isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Undefined());
  }
  VariableContext<T> nContext = VariableContext<T>('n', &context);
  Evaluation<T> * result = new Complex<T>(emptySequenceValue());
  for (int i = (int)start; i <= (int)end; i++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex<T>(Complex<T>::Undefined());
    }
    nContext.setApproximationForVariable((T)i);
    Evaluation<T> * expression = operand(0)->privateApproximate(T(), nContext, angleUnit);
    Evaluation<T> * newResult = evaluateWithNextTerm(T(), result, expression);
    delete result;
    delete expression;
    result = newResult;
    if (result == nullptr) {
      return new Complex<T>(Complex<T>::Float(NAN));
    }
  }
  return result;
}

}
