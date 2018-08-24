#include <poincare/floor.h>
#include <poincare/floor_layout_node.h>
#include <poincare/symbol.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ExpressionNode::Type Floor::type() const {
  return Type::Floor;
}

Expression * Floor::clone() const {
  Floor * c = new Floor(m_operands, true);
  return c;
}

Expression Floor::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression * op = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
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
std::complex<T> Floor::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::Undefined();
  }
  return Complex<T>(std::floor(c.real()));
}

LayoutRef Floor::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return FloorLayoutRef(m_operands[0]->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

}
