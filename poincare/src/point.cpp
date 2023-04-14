#include <poincare/layout_helper.h>
#include <poincare/point.h>
#include <poincare/point_evaluation.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

Layout PointNode::createLayout(Preferences::PrintFloatMode floatDisplayMode,
                               int significantDigits, Context* context) const {
  return LayoutHelper::Prefix(Point(this), floatDisplayMode, significantDigits,
                              k_prefix, context);
}

int PointNode::serialize(char* buffer, int bufferSize,
                         Preferences::PrintFloatMode floatDisplayMode,
                         int significantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode,
                                     significantDigits, k_prefix);
}

Expression PointNode::shallowReduce(const ReductionContext& reductionContext) {
  return Point(this).shallowReduce(reductionContext);
}

template <typename T>
Evaluation<T> PointNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Coordinate2D<T> xy = Point(this).approximate2D<T>(
      approximationContext.context(), approximationContext.complexFormat(),
      approximationContext.angleUnit());
  if (std::isnan(xy.x()) || std::isnan(xy.y())) {
    return Complex<T>::Undefined();
  }
  return PointEvaluation<T>::Builder(xy.x(), xy.y());
}

Expression Point::shallowReduce(ReductionContext reductionContext) {
  Expression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
      SimplificationHelper::UnitReduction::BanUnits,
      SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
      SimplificationHelper::ListReduction::DistributeOverLists);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

}  // namespace Poincare
