#include <poincare/sequence.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <poincare/variable_context.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}

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

Evaluation * Sequence::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * aInput = m_args[1]->evaluate(context, angleUnit);
  Evaluation * bInput = m_args[2]->evaluate(context, angleUnit);
  float start = aInput->toFloat();
  float end = bInput->toFloat();
  delete aInput;
  delete bInput;
  if (isnan(start) || isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return new Complex(Complex::Float(NAN));
  }
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  Evaluation * result = new Complex(Complex::Float(emptySequenceValue()));
  for (int i = (int)start; i <= (int)end; i++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex(Complex::Float(NAN));
    }
    Complex iExpression = Complex::Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    Evaluation * expression = m_args[0]->evaluate(nContext, angleUnit);
    Evaluation * newResult = evaluateWithNextTerm(result, expression);
    delete result;
    delete expression;
    result = newResult;
  }
  return result;
}

}
