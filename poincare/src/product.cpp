#include <poincare/product.h>
#include <poincare/symbol.h>
#include <poincare/float.h>
#include <poincare/variable_context.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
#include "layout/product_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

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
    Float iExpression = Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    result = result*m_args[0]->approximate(nContext, angleUnit);
  }
  return result;
}

ExpressionLayout * Product::createLayout(DisplayMode displayMode) const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout(displayMode);
  return new ProductLayout(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(displayMode), m_args[0]->createLayout(displayMode));
}
