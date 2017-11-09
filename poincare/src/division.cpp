extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/tangent.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include "layout/fraction_layout.h"

namespace Poincare {

Expression::Type Division::type() const {
  return Type::Division;
}

Expression * Division::clone() const {
  return new Division(m_operands, true);
}

Expression * Division::shallowReduce(Context& context, AngleUnit angleUnit) {
  Power * p = new Power(operand(1), new Rational(-1), false);
  Multiplication * m = new Multiplication(operand(0), p, false);
  detachOperands();
  p->deepReduce(context, angleUnit);
  replaceWith(m, true);
  return m->shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> Division::compute(const Complex<T> c, const Complex<T> d) {
  T norm = d.a()*d.a() + d.b()*d.b();
  /* We handle the case of c and d pure real numbers apart. Even if the complex
   * division is mathematically correct on real numbers, it requires more
   * operations and is thus more likely to propagate errors due to float exact
   * representation. */
  if (d.b() == 0 && c.b() == 0) {
    return Complex<T>::Float(c.a()/d.a());
  }
  return Complex<T>::Cartesian((c.a()*d.a()+c.b()*d.b())/norm, (d.a()*c.b()-c.a()*d.b())/norm);
}

template<typename T> Evaluation<T> * Division::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * n) {
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnComplexAndMatrix(c, inverse);
  delete inverse;
  return result;
}

template<typename T> Evaluation<T> * Division::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {
  if (m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Evaluation<T> * inverse = n->createInverse();
  Evaluation<T> * result = Multiplication::computeOnMatrices(m, inverse);
  delete inverse;
  return result;
}

ExpressionLayout * Division::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  const Expression * numerator = operand(0)->type() == Type::Parenthesis ? operand(0)->operand(0) : operand(0);
  const Expression * denominator = operand(1)->type() == Type::Parenthesis ? operand(1)->operand(0) : operand(1);
  return new FractionLayout(numerator->createLayout(floatDisplayMode, complexFormat), denominator->createLayout(floatDisplayMode, complexFormat));
}

int Division::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  bool numeratorRequireParenthesis = operand(0)->type() == Type::Multiplication || operand(0)->type() == Type::Addition || operand(0)->type() == Type::Subtraction || operand(0)->type() == Type::Opposite;
  if (numeratorRequireParenthesis) {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  numberOfChar += operand(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (numeratorRequireParenthesis) {
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  buffer[numberOfChar++] = '/';
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  bool denominatorRequireParenthesis = operand(1)->type() == Type::Multiplication || operand(1)->type() == Type::Addition || operand(1)->type() == Type::Subtraction || operand(1)->type() == Type::Opposite;
  if (denominatorRequireParenthesis) {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  numberOfChar += operand(1)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar);
  if (denominatorRequireParenthesis) {

    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

}
