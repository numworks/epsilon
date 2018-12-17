#ifndef POINCARE_RANDOM_H
#define POINCARE_RANDOM_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class RandomNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RandomNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Random";
  }
#endif

  // Properties
  Type type() const override { return Type::Random; }
  Sign sign() const override { return Sign::Positive; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<float>();
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return templateApproximate<double>();
  }
  template <typename T> Evaluation<T> templateApproximate() const;
};

class Random final : public Expression {
friend class RandomNode;
public:
  Random(const RandomNode * n) : Expression(n) {}
  static Random Builder() { return Random(); }
  static Expression UntypedBuilder(Expression children) { return Builder(); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("random", 0, &UntypedBuilder);

  template<typename T> static T random();
private:
  Random() : Expression(TreePool::sharedPool()->createTreeNode<RandomNode>()) {}
  Expression setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
