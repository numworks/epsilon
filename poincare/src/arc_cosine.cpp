#include <poincare/arc_cosine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper ArcCosine::s_functionHelper;

int ArcCosineNode::numberOfChildren() const { return ArcCosine::s_functionHelper.numberOfChildren(); }

Layout ArcCosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ArcCosine(this), floatDisplayMode, numberOfSignificantDigits, ArcCosine::s_functionHelper.name());
}

int ArcCosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcCosine::s_functionHelper.name());
}

Expression ArcCosineNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return ArcCosine(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> ArcCosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::acos(c);
  /* acos has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued on
   * this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, -1+0i[ (warning: acos takes the other side of the cut values on
   * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to comply with
   * acos(-x) = Pi - acos(x) and tan(arccos(x)) = sqrt(1-x^2)/x. */
  if (c.imag() == 0 && c.real() > 1) {
    result.imag(-result.imag()); // other side of the cut
  }
  result = Trigonometry::RoundToMeaningfulDigits(result, c);
  return Complex<T>(Trigonometry::ConvertRadianToAngleUnit(result, angleUnit));
}

Expression ArcCosine::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceInverseFunction(*this, context, angleUnit);
}

}
