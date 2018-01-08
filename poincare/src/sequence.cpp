#include <poincare/sequence.h>
#include <poincare/complex.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

ExpressionLayout * Sequence::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return createSequenceLayoutWithArgumentLayouts(operand(0)->createLayout(floatDisplayMode, complexFormat), operand(1)->createLayout(floatDisplayMode, complexFormat), operand(2)->createLayout(floatDisplayMode, complexFormat));
}

template<typename T>
Expression * Sequence::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression * aInput = operand(1)->approximate<T>(context, angleUnit);
  Expression * bInput = operand(2)->approximate<T>(context, angleUnit);
  T start = aInput->type() == Type::Complex ? static_cast<Complex<T> *>(aInput)->toScalar() : NAN;
  T end = bInput->type() == Type::Complex ? static_cast<Complex<T> *>(bInput)->toScalar() : NAN;
  delete aInput;
  delete bInput;
  if (std::isnan(start) || std::isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  VariableContext<T> nContext = VariableContext<T>('n', &context);
  Symbol nSymbol('n');
  Expression * result = new Complex<T>(Complex<T>::Float(emptySequenceValue()));
  for (int i = (int)start; i <= (int)end; i++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex<T>(Complex<T>::Float(NAN));
    }
    Complex<T> iExpression = Complex<T>::Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol, nContext);
    Expression * expression = operand(0)->approximate<T>(nContext, angleUnit);
    Expression * newResult = evaluateWithNextTerm(T(), result, expression);
    delete result;
    delete expression;
    result = newResult;
  }
  return result;
}

}
