#ifndef POINCARE_CONFIDENCE_INTERVAL_H
#define POINCARE_CONFIDENCE_INTERVAL_H

#include <poincare/expression.h>

namespace Poincare {

class ConfidenceIntervalNode : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ConfidenceIntervalNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ConfidenceInterval";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::ConfidenceInterval; }
  int polynomialDegree(Context & context, const char * symbolName) const override { return -1; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class SimplePredictionIntervalNode final : public ConfidenceIntervalNode {
public:
private:
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
};

class ConfidenceInterval : public Expression {
  friend class SimplePredictionInterval;
public:
  ConfidenceInterval(const ConfidenceIntervalNode * n) : Expression(n) {}
  static ConfidenceInterval Builder(Expression child0, Expression child1) { return ConfidenceInterval(child0, child1); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0), children.childAtIndex(1)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("confidence", 2, &UntypedBuilder);

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  ConfidenceInterval(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<ConfidenceIntervalNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
  constexpr static int k_maxNValue = 300;
};

class SimplePredictionInterval final : public ConfidenceInterval {
public:
  SimplePredictionInterval(const SimplePredictionIntervalNode * n) : ConfidenceInterval(n) {}
  static SimplePredictionInterval Builder(Expression child0, Expression child1) { return SimplePredictionInterval(child0, child1); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0), children.childAtIndex(1)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("prediction", 2, &UntypedBuilder);
private:
  SimplePredictionInterval(Expression child0, Expression child1) : ConfidenceInterval(TreePool::sharedPool()->createTreeNode<SimplePredictionIntervalNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
};

}

#endif
