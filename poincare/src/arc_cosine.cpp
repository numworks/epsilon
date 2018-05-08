#include <poincare/arc_cosine.h>
#include <poincare/trigonometry.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ArcCosine::type() const {
  return Type::ArcCosine;
}

Expression * ArcCosine::clone() const {
  ArcCosine * a = new ArcCosine(m_operands, true);
  return a;
}

Expression * ArcCosine::shallowReduce(Context& context, AngleUnit angleUnit) {
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
std::complex<T> ArcCosine::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  std::complex<T> result = std::acos(c);
  /* acos has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued on
   * this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, -1+0i[ (warning: acos takes the other side of the cut values on
   * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to comply with
   * acos(-x) = Pi - acos(x) and tan(arccos(x)) = sqrt(1-x^2)/x. */
  if (c.imag() == 0 && c.real() > 1) {
    result.imag(-result.imag()); // other side of the cut
  }
  result = Trigonometry::RoundToMeaningfulDigits(result);
  return Trigonometry::ConvertRadianToAngleUnit(result, angleUnit);
}

}
