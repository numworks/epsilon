#include <poincare/frac_part.h>
#include <poincare/simplification_engine.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type FracPart::type() const {
  return Type::FracPart;
}

Expression * FracPart::clone() const {
  FracPart * c = new FracPart(m_operands, true);
  return c;
}

Expression * FracPart::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  if (op->type() != Type::Rational) {
    return this;
  }
  Rational * r = static_cast<Rational *>(op);
  IntegerDivision div = Integer::Division(r->numerator(), r->denominator());
  return replaceWith(new Rational(div.remainder, r->denominator()), true);
}

template<typename T>
Complex<T> FracPart::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(c.a()-std::floor(c.a()));
}

}


