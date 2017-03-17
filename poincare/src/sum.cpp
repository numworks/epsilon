#include <poincare/sum.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <poincare/variable_context.h>
#include <poincare/addition.h>
#include "layout/sum_layout.h"
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}

namespace Poincare {

Sum::Sum() :
  Function("sum")
{
}

bool Sum::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 3);
}

Expression::Type Sum::type() const {
  return Type::Sum;
}

Expression * Sum::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  Sum * s = new Sum();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

float Sum::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  int start = m_args[1]->approximate(context, angleUnit);
  int end = m_args[2]->approximate(context, angleUnit);
  float result = 0.0f;
  for (int i = start; i <= end; i++) {
    Complex iExpression = Complex::Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    result += m_args[0]->approximate(nContext, angleUnit);
  }
  return result;
}

ExpressionLayout * Sum::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout(floatDisplayMode, complexFormat);
  return new SumLayout(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(floatDisplayMode, complexFormat), m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

Expression * Sum::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float start = m_args[1]->approximate(context, angleUnit);
  float end = m_args[2]->approximate(context, angleUnit);
  if (isnan(start) || isnan(end)) {
    return new Complex(Complex::Float(NAN));
  }
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  Expression * result = new Complex(Complex::Float(0.0f));
  for (int i = (int)start; i <= (int)end; i++) {
    Complex * iExpression = new Complex(Complex::Float(i));
    nContext.setExpressionForSymbolName(iExpression, &nSymbol);
    delete iExpression;
    Expression * operands[2];
    operands[0] = result;
    operands[1] = m_args[0]->evaluate(nContext, angleUnit);
    Expression * addition = new Addition(operands, true);
    Expression * newResult = addition->evaluate(context, angleUnit);
    delete result;
    delete operands[1];
    result = newResult;
    delete addition;
  }
  return result;
}

}
