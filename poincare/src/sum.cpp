#include <poincare/sum.h>
#include <poincare/symbol.h>
#include <poincare/float.h>
#include "layout/sum_layout.h"
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

Sum::Sum() :
  Function("sum")
{
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

float Sum::approximate(Context& context) const {
  NContext nContext = NContext(&context);
  Symbol nSymbol = Symbol('n');
  int start = m_args[1]->approximate(context);
  int end = m_args[2]->approximate(context);
  float result = 0.0f;
  for (int i = start; i <= end; i++) {
    Float iExpression = Float(i);
    nContext.setExpressionForSymbolName(&iExpression, &nSymbol);
    result += m_args[0]->approximate(nContext);
  }
  return result;
}

ExpressionLayout * Sum::createLayout() const {
  ExpressionLayout ** childrenLayouts = (ExpressionLayout **)malloc(2*sizeof(ExpressionLayout *));
  childrenLayouts[0] = new StringLayout("n=", 2);
  childrenLayouts[1] = m_args[1]->createLayout();
  return new SumLayout(new HorizontalLayout(childrenLayouts, 2), m_args[2]->createLayout(), m_args[0]->createLayout());
}
