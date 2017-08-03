#include <poincare/sequence.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <poincare/variable_context.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Sequence::Sequence(const char * name) :
  Function(name, 3)
{
}

ExpressionLayout * Sequence::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout(floatDisplayMode, complexFormat);
  return createSequenceLayoutWithArgumentLayouts(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(floatDisplayMode, complexFormat), m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

template<typename T>
Evaluation<T> * Sequence::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * aInput = m_args[1]->evaluate<T>(context, angleUnit);
  Evaluation<T> * bInput = m_args[2]->evaluate<T>(context, angleUnit);
  T start = aInput->toScalar();
  T end = bInput->toScalar();
  delete aInput;
  delete bInput;
  if (isnan(start) || isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  VariableContext<T> nContext = VariableContext<T>('n', &context);
  Symbol nSymbol = Symbol('n');
  Evaluation<T> * result = new Complex<T>(Complex<T>::Float(emptySequenceValue()));
  for (int i = (int)start; i <= (int)end; i++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex<T>(Complex<T>::Float(NAN));
    }
    Complex<T> iExpression = Complex<T>::Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    Evaluation<T> * expression = m_args[0]->evaluate<T>(nContext, angleUnit);
    Evaluation<T> * newResult = evaluateWithNextTerm(result, expression);
    delete result;
    delete expression;
    result = newResult;
  }
  return result;
}

}
