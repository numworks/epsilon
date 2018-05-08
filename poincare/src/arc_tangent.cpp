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
  /* atan has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-i+0, -i*inf+0[ (warning: atan takes the other side of the cut values on
   * ]-i+0, -i*inf+0[) and choose the values on ]-inf*i, -i[ to comply with
   * atan(-x) = -atan(x) and sin(arctan(x)) = x/sqrt(1+x^2). */
  if (c.real() == 0 && c.imag() < -1) {
    result.real(-result.real()); // other side of the cut
  }
  result = Trigonometry::RoundToMeaningfulDigits(result);
  return Trigonometry::ConvertRadianToAngleUnit(result, angleUnit);
}

}
