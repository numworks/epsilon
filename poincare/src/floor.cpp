#include <poincare/floor.h>
#include "layout/floor_layout.h"
#include <poincare/symbol.h>
#include <poincare/simplification_engine.h>
#include <poincare/rational.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Floor::type() const {
  return Type::Floor;
}

Expression * Floor::clone() const {
  Floor * c = new Floor(m_operands, true);
  return c;
}

Expression * Floor::shallowReduce(Context& context, AngleUnit angleUnit) {
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
      return replaceWith(new Rational(3), true);
    }
    if (s->name() == Ion::Charset::Exponential) {
      return replaceWith(new Rational(2), true);
    }
  }
  if (op->type() != Type::Rational) {
    return this;
  }
  Rational * r = static_cast<Rational *>(op);
  IntegerDivision div = Integer::Division(r->numerator(), r->denominator());
  return replaceWith(new Rational(div.quotient), true);
}

template<typename T>
Complex<T> Floor::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::floor(c.a()));
}

ExpressionLayout * Floor::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new FloorLayout(m_operands[0]->createLayout(floatDisplayMode, complexFormat));
}

}
