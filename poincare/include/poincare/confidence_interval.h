#ifndef POINCARE_CONFIDENCE_INTERVAL_H
#define POINCARE_CONFIDENCE_INTERVAL_H

#include <poincare/expression.h>

namespace Poincare {

class ConfidenceIntervalNode : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ConfidenceIntervalNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "ConfidenceInterval";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::ConfidenceInterval; }
  int polynomialDegree(char symbolName) const override { return -1; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  virtual const char * name() const { return "confidence"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class SimplePredictionIntervalNode final : public ConfidenceIntervalNode {
public:
private:
  const char * name() const override { return "prediction"; }
};

class ConfidenceInterval : public Expression {
public:
  ConfidenceInterval();
  ConfidenceInterval(const ConfidenceIntervalNode * n) : Expression(n) {}
  ConfidenceInterval(Expression child1, Expression child2) : ConfidenceInterval() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
private:
  constexpr static int k_maxNValue = 300;
};

class SimplePredictionInterval final : public ConfidenceInterval {
public:
  SimplePredictionInterval() : ConfidenceInterval(static_cast<SimplePredictionIntervalNode *>(TreePool::sharedPool()->createTreeNode<SimplePredictionIntervalNode>())) {}
  SimplePredictionInterval(const SimplePredictionIntervalNode * n) : ConfidenceInterval(n) {}
  SimplePredictionInterval(Expression child1, Expression child2) : SimplePredictionInterval() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }
};

}

#endif
