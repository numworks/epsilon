#include <poincare/real_part.h>
#include <poincare/simplification_helper.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type RealPart::type() const {
  return Type::RealPart;
}

Expression * RealPart::clone() const {
  RealPart * a = new RealPart(m_operands, true);
  return a;
}

Expression RealPart::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
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
std::complex<T> RealPart::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return std::real(c);
}

}


