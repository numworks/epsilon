#include <poincare/opposite.h>
#include <poincare/complex.h>
#include <poincare/multiplication.h>
#include <poincare/simplification_engine.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>
#include "layout/horizontal_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/string_layout.h"

namespace Poincare {

Expression::Type Opposite::type() const {
  return Type::Opposite;
}

Expression * Opposite::clone() const {
  Opposite * o = new Opposite(m_operands, true);
  return o;
}

int Opposite::polynomialDegree(char symbolName) const {
  return operand(0)->polynomialDegree(symbolName);
}

Expression::Sign Opposite::sign() const {
  if (operand(0)->sign() == Sign::Positive) {
    return Sign::Negative;
  }
  if (operand(0)->sign() == Sign::Negative) {
    return Sign::Positive;
  }
  return Sign::Unknown;
}

/* Layout */

bool Opposite::needParenthesisWithParent(const Expression * e) const {
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 7);
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

ExpressionLayout * Opposite::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * children_layouts[2];
  char string[2] = {'-', '\0'};
  children_layouts[0] = new StringLayout(string, 1);
  children_layouts[1] = operand(0)->type() == Type::Opposite ? new ParenthesisLayout(operand(0)->createLayout(floatDisplayMode, complexFormat)) : operand(0)->createLayout(floatDisplayMode, complexFormat);
  return new HorizontalLayout(children_layouts, 2);
}

int Opposite::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (bufferSize == 1) { return 0; }
  buffer[numberOfChar++] = '-';
  numberOfChar += operand(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

}

template Poincare::Complex<float> Poincare::Opposite::compute<float>(Poincare::Complex<float>, AngleUnit angleUnit);
template Poincare::Complex<double> Poincare::Opposite::compute<double>(Poincare::Complex<double>, AngleUnit angleUnit);
