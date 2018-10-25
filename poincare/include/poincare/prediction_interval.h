#ifndef POINCARE_PREDICTION_INTERVAL_H
#define POINCARE_PREDICTION_INTERVAL_H

#include <poincare/expression.h>

namespace Poincare {

class PredictionIntervalNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(PredictionIntervalNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "PredictionInterval";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::PredictionInterval; }
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

class PredictionInterval final : public Expression {
public:
  PredictionInterval(const PredictionIntervalNode * n) : Expression(n) {}
  static PredictionInterval Builder(Expression child0, Expression child1) { return PredictionInterval(child0, child1); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0), children.childAtIndex(1)); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("prediction95", 2, &UntypedBuilder);

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  PredictionInterval(Expression child0, Expression child1) : Expression(TreePool::sharedPool()->createTreeNode<PredictionIntervalNode>()) {
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
  }
};

}

#endif
