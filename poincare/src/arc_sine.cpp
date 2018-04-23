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
std::complex<T> ArcSine::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  std::complex<T> result = std::asin(c);
  if (angleUnit == AngleUnit::Degree && result.imag() == 0.0) {
    result *= 180/M_PI;
  }
  return result;
}

}
