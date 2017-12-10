#include <poincare/ceiling.h>
#include "layout/ceiling_layout.h"
#include <cmath>
#include <ion.h>
#include <poincare/symbol.h>
#include <poincare/simplification_engine.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
}

namespace Poincare {

Expression::Type Ceiling::type() const {
  return Type::Ceiling;
}

Expression * Ceiling::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  if (op->type() == Type::Symbol) {
    Symbol * s = static_cast<Symbol *>(op);
    if (s->name() == Ion::Charset::SmallPi) {
      return replaceWith(new Rational(4), true);
    }
    if (s->name() == Ion::Charset::Exponential) {
      return replaceWith(new Rational(3), true);
    }
  }
  if (op->type() != Type::Rational) {
    return this;
  }
  Rational * r = static_cast<Rational *>(op);
  IntegerDivision div = Integer::Division(r->numerator(), r->denominator());
  if (div.remainder.isZero()) {
    return replaceWith(new Rational(div.quotient), true);
  }
  return replaceWith(new Rational(Integer::Addition(div.quotient, Integer(1))), true);
}

template<typename T>
Complex<T> Ceiling::computeOnComplex(const Complex<T> & c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::FNAN();
  }
  return Complex<T>::Float(std::ceil(c.a()));
}

ExpressionLayout * Ceiling::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new CeilingLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat), false);
}

}
