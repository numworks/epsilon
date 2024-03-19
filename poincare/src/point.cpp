#include <poincare/layout_helper.h>
#include <poincare/point.h>
#include <poincare/point_2D_layout.h>
#include <poincare/point_evaluation.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

namespace Poincare {

size_t PointNode::serialize(char* buffer, size_t bufferSize,
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
  Coordinate2D<T> xy = Point(this).approximate2D<T>(approximationContext);
  return PointEvaluation<T>::Builder(xy.x(), xy.y());
}

Expression Point::shallowReduce(ReductionContext reductionContext) {
  Expression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
      SimplificationHelper::UnitReduction::BanUnits,
      SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
      SimplificationHelper::ListReduction::DistributeOverLists,
      SimplificationHelper::PointReduction::UndefinedOnPoint,
      SimplificationHelper::UndefReduction::DoNotBubbleUpUndef,
      SimplificationHelper::DependencyReduction::DoNotBubbleUp);
  if (!e.isUninitialized()) {
    return e;
  }
  return *this;
}

Layout Point::create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                             int significantDigits, Context* context) const {
  Layout child0 = childAtIndex(0).createLayout(floatDisplayMode,
                                               significantDigits, context);
  Layout child1 = childAtIndex(1).createLayout(floatDisplayMode,
                                               significantDigits, context);
  return JuniorLayout::Create(KPoint2DL(KA, KB), {.KA = child0, .KB = child1});
}

}  // namespace Poincare
