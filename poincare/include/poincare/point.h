#ifndef POINCARE_POINT_H
#define POINCARE_POINT_H

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>

namespace Poincare {

class PointNode : public ExpressionNode {
 public:
  // ExpressionNode
  Type type() const override { return Type::Point; }
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
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
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int significantDigits) const override;

 private:
  constexpr static char k_prefix[] = "";

  template <typename T>
  Evaluation<T> templatedApproximate(const ApproximationContext&) const;
};

class Point : public ExpressionTwoChildren<Point, PointNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;

  Expression shallowReduce(ReductionContext reductionContext);

  template <typename T>
  Coordinate2D<T> approximate2D(Context* context,
                                Preferences::ComplexFormat complexFormat,
                                Preferences::AngleUnit angleUnit,
                                bool withinReduce = false) {
    return Coordinate2D<T>(
        childAtIndex(0).approximateToScalar<T>(context, complexFormat,
                                               angleUnit, withinReduce),
        childAtIndex(1).approximateToScalar<T>(context, complexFormat,
                                               angleUnit, withinReduce));
  }
};

}  // namespace Poincare

#endif
