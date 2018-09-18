#include <poincare/cosine.h>
#include <poincare/complex.h>
#include <poincare/layout_helper.h>
#include <poincare/simplification_helper.h>
#include <cmath>

namespace Poincare {

float CosineNode::characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(Cosine(this), context, angleUnit);
}

template<typename T>
Complex<T> CosineNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::cos(angleInput);
  return Complex<T>(Trigonometry::RoundToMeaningfulDigits(res));
}

Layout CosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Cosine(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Expression CosineNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Cosine(this).shallowReduce(context, angleUnit);
}

Cosine::Cosine() : Expression(TreePool::sharedPool()->createTreeNode<CosineNode>()) {}

Expression Cosine::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  Expression op = childAtIndex(0);
  if (op.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceDirectFunction(*this, context, angleUnit);
}

}
