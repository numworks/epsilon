#include <poincare/imaginary_part.h>
#include <poincare/complex.h>
#include <poincare/simplification_engine.h>
#include <poincare/rational.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

Expression::Type ImaginaryPart::type() const {
  return Type::ImaginaryPart;
}

Expression * ImaginaryPart::clone() const {
  ImaginaryPart * a = new ImaginaryPart(m_operands, true);
  return a;
}


Expression * ImaginaryPart::shallowReduce(Context& context, AngleUnit angleUnit) {
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
    return replaceWith(new Rational(0), true);
  }
  return this;
}

template<typename T>
Complex<T> ImaginaryPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  return Complex<T>::Float(c.b());
}

}


