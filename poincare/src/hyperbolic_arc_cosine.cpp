#include <poincare/hyperbolic_arc_cosine.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcCosine::type() const {
  return Type::HyperbolicArcCosine;
}

Expression * HyperbolicArcCosine::clone() const {
  HyperbolicArcCosine * a = new HyperbolicArcCosine(m_operands, true);
  return a;
}

Expression HyperbolicArcCosine::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression * op = childAtIndex(0);
  if (op->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
  return this;
}

template<typename T>
std::complex<T> HyperbolicArcCosine::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::acosh(c);
  /* asinh has a branch cut on ]-inf, 1]: it is then multivalued
   * on this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, 1+0i] (warning: atanh takes the other side of the cut values on
   * ]-inf-0i, 1-0i[).*/
  return Trigonometry::RoundToMeaningfulDigits(result);
}

}
