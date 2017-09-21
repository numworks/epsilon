extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/parenthesis.h>
#include "layout/parenthesis_layout.h"

namespace Poincare {


Expression::Type Parenthesis::type() const {
  return Type::Parenthesis;
}

Expression * Parenthesis::clone() const {
  Parenthesis * o = new Parenthesis(m_operands, true);
  return o;
}

bool Parenthesis::isCommutative() const {
  return false;
}

ExpressionLayout * Parenthesis::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new ParenthesisLayout(m_operand->createLayout(floatDisplayMode, complexFormat));
}

template<typename T>
Evaluation<T> * Parenthesis::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  return operand(0)->evaluate<T>(context, angleUnit);
}

}
