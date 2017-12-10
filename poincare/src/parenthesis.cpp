#include <poincare/parenthesis.h>

extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {


Expression::Type Parenthesis::type() const {
  return Type::Parenthesis;
}

int Parenthesis::polynomialDegree(char symbolName) const {
  return operand(0)->polynomialDegree(symbolName);
}

ExpressionLayout * Parenthesis::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return LayoutEngine::createParenthesedLayout(operand(0)->createLayout(floatDisplayMode, complexFormat), false);
}

Expression * Parenthesis::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  return replaceWith(editableOperand(0), true);
}

}
