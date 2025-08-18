#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include "evaluation.h"
#include "old_expression.h"

namespace Poincare {

class RandomNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "random";

  // PoolObject
  size_t size() const override { return sizeof(RandomNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override { stream << "Random"; }
#endif

  // Properties
  Type otype() const override { return Type::Random; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::Troolean::True;
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
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

class Random final : public ExpressionNoChildren<Random, RandomNode> {
  friend class RandomNode;

 public:
  using ExpressionBuilder::ExpressionBuilder;
  static OExpression UntypedBuilder(OExpression children) {
    assert(children.otype() == ExpressionNode::Type::OList);
    return Builder();
  }
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("random", 0, &UntypedBuilder);
};

}  // namespace Poincare

#endif
