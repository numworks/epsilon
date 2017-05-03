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

float Sequence::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  float start = m_args[1]->approximate(context, angleUnit);
  float end = m_args[2]->approximate(context, angleUnit);
  if (isnan(start) || isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return NAN;
  }
  float result = emptySequenceValue();
  for (int i = start; i <= end; i++) {
    if (shouldStopProcessing()) {
      return NAN;
    }
    Complex iExpression = Complex::Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    result = approximateWithNextTerm(result, m_args[0]->approximate(nContext, angleUnit));
  }
  return result;
}

ExpressionLayout * Sequence::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout(floatDisplayMode, complexFormat);
  return createSequenceLayoutWithArgumentLayouts(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(floatDisplayMode, complexFormat), m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

Expression * Sequence::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float start = m_args[1]->approximate(context, angleUnit);
  float end = m_args[2]->approximate(context, angleUnit);
  if (isnan(start) || isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return new Complex(Complex::Float(NAN));
  }
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  Expression * result = new Complex(Complex::Float(emptySequenceValue()));
  for (int i = (int)start; i <= (int)end; i++) {
    if (shouldStopProcessing()) {
      delete result;
      return new Complex(Complex::Float(NAN));
    }
    Complex * iExpression = new Complex(Complex::Float(i));
    nContext.setExpressionForSymbolName(iExpression, &nSymbol);
    delete iExpression;
    Expression * operands[2];
    operands[0] = result;
    operands[1] = m_args[0]->evaluate(nContext, angleUnit);
    Expression * newResult = evaluateWithNextTerm(operands, context, angleUnit);
    delete result;
    delete operands[1];
    result = newResult;
  }
  return result;
}

}
