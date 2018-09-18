#include <poincare/arc_sine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

Layout ArcSineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ArcSine(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression ArcSineNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return ArcSine(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> ArcSineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::asin(c);
  /* asin has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued on
   * this cut. We followed the convention chosen by the lib c++ of llvm on
   * ]-inf+0i, -1+0i[ (warning: asin takes the other side of the cut values on
   * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to comply with
   * asin(-x) = -asin(x) and tan(arcsin(x)) = x/sqrt(1-x^2). */
  if (c.imag() == 0 && c.real() > 1) {
    result.imag(-result.imag()); // other side of the cut
  }
  result = Trigonometry::RoundToMeaningfulDigits(result);
  return Complex<T>(Trigonometry::ConvertRadianToAngleUnit(result, angleUnit));
}

ArcSine::ArcSine() : Expression(TreePool::sharedPool()->createTreeNode<ArcSineNode>()) {}

Expression ArcSine::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
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
