#include <poincare/real_part.h>
#include <poincare/complex.h>
#include <poincare/simplification_engine.h>
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

Expression * RealPart::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  if (op->type() == Type::Rational) {
    return replaceWith(op, true);
  }
  return this;
}

template<typename T>
Complex<T> RealPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.a());
}

}


