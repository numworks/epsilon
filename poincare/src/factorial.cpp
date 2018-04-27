#include <poincare/factorial.h>
#include "layout/string_layout.h"
#include "layout/horizontal_layout.h"
#include <poincare/rational.h>
#include <poincare/undefined.h>
#include <poincare/symbol.h>
#include <poincare/simplification_engine.h>
#include <poincare/parenthesis.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Factorial::Factorial(const Expression * argument, bool clone) :
  StaticHierarchy<1>(&argument, clone)
{
}

Expression::Type Factorial::type() const {
  return Type::Factorial;
}

Expression * Factorial::clone() const {
  Factorial * a = new Factorial(m_operands[0], true);
  return a;
}

/* Layout */

bool Factorial::needParenthesisWithParent(const Expression * e) const {
  return e->type() == Type::Factorial;
}

/* Simplification */

Expression * Factorial::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  if (operand(0)->type() == Type::Rational) {
    Rational * r = static_cast<Rational *>(editableOperand(0));
    if (!r->denominator().isOne() || r->sign() == Sign::Negative) {
      return replaceWith(new Undefined(), true);
    }
    if (Integer(k_maxOperandValue).isLowerThan(r->numerator())) {
      return this;
    }
    Rational * fact = new Rational(Integer::Factorial(r->numerator()));
    return replaceWith(fact, true);
  }
  if (operand(0)->type() == Type::Symbol) {
    Symbol * s = static_cast<Symbol *>(editableOperand(0));
    if (s->name() == Ion::Charset::SmallPi || s->name() == Ion::Charset::Exponential) {
      return replaceWith(new Undefined(), true);
    }
  }
  return this;
}

Expression * Factorial::shallowBeautify(Context& context, AngleUnit angleUnit) {
  // +(a,b)! ->(+(a,b))!
  if (operand(0)->type() == Type::Addition || operand(0)->type() == Type::Multiplication || operand(0)->type() == Type::Power) {
    const Expression * o[1] = {operand(0)};
    Parenthesis * p = new Parenthesis(o, true);
    replaceOperand(operand(0), p, true);
  }
  return this;
}

template<typename T>
Complex<T> Factorial::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  T n = c.a();
  if (c.b() != 0 || std::isnan(n) || n != (int)n || n < 0) {
    return Complex<T>::Float(NAN);
  }
  T result = 1;
  for (int i = 1; i <= (int)n; i++) {
    result *= (T)i;
    if (std::isinf(result)) {
      return Complex<T>::Float(result);
    }
  }
  return Complex<T>::Float(std::round(result));
}

ExpressionLayout * Factorial::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout * childrenLayouts[2];
  childrenLayouts[0] = operand(0)->createLayout(floatDisplayMode, complexFormat);
  childrenLayouts[1] = new StringLayout("!", 1);
  return new HorizontalLayout(childrenLayouts, 2);
}

int Factorial::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int numberOfChar = 0;
  if (operand(0)->needParenthesisWithParent(this)) {
    buffer[numberOfChar++] = '(';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  numberOfChar += operand(0)->writeTextInBuffer(buffer+numberOfChar, bufferSize-numberOfChar, numberOfSignificantDigits);
  if (operand(0)->needParenthesisWithParent(this)) {
    buffer[numberOfChar++] = ')';
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  }
  if (numberOfChar >= bufferSize-1) {
    return numberOfChar;
  }
  buffer[numberOfChar++] = '!';
  buffer[numberOfChar] = 0;
  return numberOfChar;
}

#if 0
int Factorial::simplificationOrderGreaterType(const Expression * e) const {
  if (SimplificationOrder(operand(0),e) == 0) {
    return 1;
  }
  return SimplificationOrder(operand(0), e);
}

int Factorial::simplificationOrderSameType(const Expression * e) const {
  return SimplificationOrder(operand(0), e->operand(0));
}
#endif

}
