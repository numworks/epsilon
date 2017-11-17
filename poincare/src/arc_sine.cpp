#include <poincare/arc_sine.h>
#include <poincare/trigonometry.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ArcSine::type() const {
  return Type::ArcSine;
}

Expression * ArcSine::clone() const {
  ArcSine * a = new ArcSine(m_operands, true);
  return a;
}

Expression * ArcSine::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceInverseFunction(this, context, angleUnit);
}

template<typename T>
Complex<T> ArcSine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  T result = std::asin(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex<T>::Float(result*180/M_PI);
  }
  return Complex<T>::Float(result);
}

}
