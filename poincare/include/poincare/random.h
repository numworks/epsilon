#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include <poincare/evaluation.h>
#include <poincare/expression.h>

namespace Poincare {

class RandomNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "random";

  // TreeNode
  size_t size() const override { return sizeof(RandomNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Random"; }
#endif

  // Properties
  Type type() const override { return Type::Random; }
  TrinaryBoolean isPositive(Context* context) const override {
    return TrinaryBoolean::True;
  }

 private:
  // Simplification
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override;
  int serialize(char* buffer, int bufferSize,
                Preferences::PrintFloatMode floatDisplayMode,
                int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templateApproximate(
      const ApproximationContext& approximationContext) const;
};

class Random final : public ExpressionNoChildren<Random, RandomNode> {
  friend class RandomNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;
  static Expression UntypedBuilder(Expression children) {
    assert(children.type() == ExpressionNode::Type::List);
    return Builder();
  }
  constexpr static Expression::FunctionHelper s_functionHelper =
      Expression::FunctionHelper("random", 0, &UntypedBuilder);

  template <typename T>
  static T random();
};

}  // namespace Poincare

#endif
