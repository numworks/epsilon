#include <poincare/product.h>
#include <poincare/symbol.h>
#include <poincare/complex.h>
#include <poincare/multiplication.h>
#include <poincare/variable_context.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/product_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}

namespace Poincare {

Product::Product() :
  Function("product", 3)
{
}

Expression::Type Product::type() const {
  return Type::Product;
}

Expression * Product::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Product * p = new Product();
  p->setArgument(newOperands, numberOfOperands, cloneOperands);
  return p;
}

float Product::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  float start = m_args[1]->approximate(context, angleUnit);
  float end = m_args[2]->approximate(context, angleUnit);
  if (start != (int)start || end != (int)end) {
    return NAN;
  }
  float result = 1.0f;
  for (int i = start; i <= end; i++) {
    Complex iExpression = Complex::Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    result = result*m_args[0]->approximate(nContext, angleUnit);
  }
  return result;
}

ExpressionLayout * Product::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout(floatDisplayMode, complexFormat);
  return new ProductLayout(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(floatDisplayMode, complexFormat), m_args[0]->createLayout(floatDisplayMode, complexFormat));
}

Expression * Product::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float start = m_args[1]->approximate(context, angleUnit);
  float end = m_args[2]->approximate(context, angleUnit);
  if (isnan(start) || isnan(end)) {
    return new Complex(Complex::Float(NAN));
  }
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  Expression * result = new Complex(Complex::Float(1.0f));
  for (int i = (int)start; i <= (int)end; i++) {
    Complex * iExpression = new Complex(Complex::Float(i));
    nContext.setExpressionForSymbolName(iExpression, &nSymbol);
    delete iExpression;
    Expression * operands[2];
    operands[0] = result;
    operands[1] = m_args[0]->evaluate(nContext, angleUnit);
    Expression * multiplication = new Multiplication(operands, true);
    Expression * newResult = multiplication->evaluate(context, angleUnit);
    delete result;
    delete operands[1];
    result = newResult;
    delete multiplication;
  }
  return result;
}

}
