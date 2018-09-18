#include <poincare/arc_tangent.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

Layout ArcTangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ArcTangent(this), floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T>
Complex<T> ArcTangentNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
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
  return Complex<T>(Trigonometry::ConvertRadianToAngleUnit(result, angleUnit));
}

Expression ArcTangentNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return ArcTangent(this).shallowReduce(context, angleUnit);
}

ArcTangent::ArcTangent() : Expression(TreePool::sharedPool()->createTreeNode<ArcTangentNode>()) {}

Expression ArcTangent::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceInverseFunction(*this, context, angleUnit);
}

}
