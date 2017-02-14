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
  Function("product")
{
}

Expression::Type Product::type() const {
  return Type::Product;
}

Expression * Product::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  Product * p = new Product();
  p->setArgument(newOperands, numberOfOperands, cloneOperands);
  return p;
}

float Product::approximate(Context& context, AngleUnit angleUnit) const {
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  int start = m_args[1]->approximate(context, angleUnit);
  int end = m_args[2]->approximate(context, angleUnit);
  float result = 1.0f;
  for (int i = start; i <= end; i++) {
    Complex iExpression = Complex(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    result = result*m_args[0]->approximate(nContext, angleUnit);
  }
  return result;
}

ExpressionLayout * Product::createLayout(FloatDisplayMode FloatDisplayMode) const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout(FloatDisplayMode);
  return new ProductLayout(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(FloatDisplayMode), m_args[0]->createLayout(FloatDisplayMode));
}

Expression * Product::evaluate(Context& context, AngleUnit angleUnit) const {
  float start = m_args[1]->approximate(context, angleUnit);
  float end = m_args[2]->approximate(context, angleUnit);
  if (isnan(start) || isnan(end)) {
    return new Complex(NAN);
  }
  VariableContext nContext = VariableContext('n', &context);
  Symbol nSymbol = Symbol('n');
  Expression * result = new Complex(1);
  for (int i = (int)start; i <= (int)end; i++) {
    Complex iExpression = Complex(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
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
