#include <poincare/parenthesis.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {


Expression::Type Parenthesis::type() const {
  return Type::Parenthesis;
}

Expression * Parenthesis::clone() const {
  Parenthesis * o = new Parenthesis(m_operands, true);
  return o;
}

int Parenthesis::polynomialDegree(char symbolName) const {
  return operand(0)->polynomialDegree(symbolName);
}

LayoutRef Parenthesis::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::createParenthesedLayout(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

Expression * Parenthesis::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  return replaceWith(editableOperand(0), true);
}

template<typename T>
Evaluation<T> * Parenthesis::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  return operand(0)->privateApproximate(T(), context, angleUnit);
}

}
