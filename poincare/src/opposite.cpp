#include <poincare/opposite.h>
#include "layout/char_layout.h"
#include "layout/horizontal_layout.h"
#include <cmath>
#include <poincare/complex.h>
#include <poincare/layout_engine.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

Expression::Type Opposite::type() const {
  return Type::Opposite;
}

Expression * Opposite::clone() const {
  Opposite * o = new Opposite(m_operands, true);
  return o;
}

template<typename T>
Complex<T> Opposite::compute(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Cartesian(-c.a(), -c.b());
}

Expression * Opposite::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  const Expression * op = operand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  detachOperand(op);
  Multiplication * m = new Multiplication(new Rational(-1), op, false);
  replaceWith(m, true);
  return m->shallowReduce(context, angleUnit);
}

ExpressionLayout * Opposite::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  HorizontalLayout * result = new HorizontalLayout(new CharLayout('-'), false);
  if (operand(0)->type() == Type::Opposite) {
    result->addOrMergeChildAtIndex(LayoutEngine::createParenthesedLayout(operand(0)->createLayout(floatDisplayMode, complexFormat), false), 1, false);
    return result;
  }
  result->addOrMergeChildAtIndex(operand(0)->createLayout(floatDisplayMode, complexFormat), 1, false);
  return result;

}

int Opposite::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (bufferSize == 1) { return 0; }
  buffer[numberOfChar++] = '-';
  numberOfChar += operand(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

}

template Poincare::Complex<float> Poincare::Opposite::compute<float>(Poincare::Complex<float>, AngleUnit angleUnit);
template Poincare::Complex<double> Poincare::Opposite::compute<double>(Poincare::Complex<double>, AngleUnit angleUnit);
