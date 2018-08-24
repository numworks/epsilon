#include <poincare/conjugate.h>
#include <poincare/simplification_helper.h>
#include <poincare/conjugate_layout_node.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Expression::Type Conjugate::type() const {
  return Type::Conjugate;
}

Expression * Conjugate::clone() const {
  Conjugate * a = new Conjugate(m_operands, true);
  return a;
}

LayoutRef Conjugate::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayoutRef(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

Expression Conjugate::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
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
  if (op->type() == Type::Rational) {
    return replaceWith(op, true);
  }
  return this;
}

template<typename T>
std::complex<T> Conjugate::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return std::conj(c);
}

}

