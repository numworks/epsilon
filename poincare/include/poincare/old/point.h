#ifndef POINCARE_POINT_H
#define POINCARE_POINT_H

#include <poincare/coordinate_2D.h>

#include "old_expression.h"

namespace Poincare {

class PointNode : public ExpressionNode {
 public:
  // ExpressionNode
  Type otype() const override { return Type::OPoint; }
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // PoolHandle
  size_t size() const override { return sizeof(PointNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "OPoint"; }
#endif
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int significantDigits) const override;

  constexpr static char k_prefix[] = "";
};

class OPoint : public ExpressionTwoChildren<OPoint, PointNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  OExpression shallowReduce(ReductionContext reductionContext);

  template <typename T>
  Coordinate2D<T> approximate2D(
      const ApproximationContext& approximationContext) {
    return Coordinate2D<T>(
        childAtIndex(0).approximateToScalar<T>(approximationContext),
        childAtIndex(1).approximateToScalar<T>(approximationContext));
  }

  Layout create2DLayout(Preferences::PrintFloatMode floatDisplayMode,
                        int significantDigits, Context* context) const;
};

}  // namespace Poincare

#endif
