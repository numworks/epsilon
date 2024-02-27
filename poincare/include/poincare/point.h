#ifndef POINCARE_POINT_H
#define POINCARE_POINT_H

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>

namespace Poincare {

class PointNode : public ExpressionNode {
 public:
  // ExpressionNode
  Type type() const override { return Type::Point; }
  OLayout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                       int significantDigits, Context* context) const override;
  Evaluation<float> approximate(
      SinglePrecision p, const ApproximationContext& context) const override {
    return templatedApproximate<float>(context);
  }
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  Evaluation<double> approximate(
      DoublePrecision p, const ApproximationContext& context) const override {
    return templatedApproximate<double>(context);
  }
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // TreeHandle
  size_t size() const override { return sizeof(PointNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Point"; }
#endif
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int significantDigits) const override;

  constexpr static char k_prefix[] = "";

 private:
  template <typename T>
  Evaluation<T> templatedApproximate(const ApproximationContext&) const;
};

class Point : public ExpressionTwoChildren<Point, PointNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  Expression shallowReduce(ReductionContext reductionContext);

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
