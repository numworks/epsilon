#include <poincare/sum.h>
#include <poincare/symbol.h>
#include <poincare/float.h>

extern "C" {
#include <assert.h>
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
