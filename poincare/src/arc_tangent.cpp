#include <poincare/arc_tangent.h>
#include <poincare/trigonometry.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ArcTangent::type() const {
  return Type::ArcTangent;
}

Expression * ArcTangent::clone() const {
  ArcTangent * a = new ArcTangent(m_operands, true);
  return a;
}

Expression * ArcTangent::shallowReduce(Context& context, AngleUnit angleUnit) {
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
std::complex<T> ArcTangent::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  std::complex<T> result = std::atan(c);
  if (angleUnit == AngleUnit::Degree && result.imag() == 0.0) {
    result *= 180/M_PI;
  }
  return result;
}

}
