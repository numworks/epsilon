#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class RandomNode final : public ExpressionNode  {
public:
  static constexpr char functionName[] = "random";

  // TreeNode
  size_t size() const override { return sizeof(RandomNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Random";
  }
#endif

  // Properties
  Type type() const override { return Type::Random; }
  Sign sign(Context * context) const override { return Sign::Positive; }
private:
  // Simplification
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<double>();
  }
  template <typename T> Evaluation<T> templateApproximate() const;
};

class Random final : public HandleNoChildren<Random, RandomNode> {
friend class RandomNode;
public:
  using Handle::Handle, Handle::Builder;
  static Expression UntypedBuilder(Expression children) { assert(children.type() == ExpressionNode::Type::List); return Builder(); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("random", 0, &UntypedBuilder);

  template<typename T> static T random();
};

}

#endif
