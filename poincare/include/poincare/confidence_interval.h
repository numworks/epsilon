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
  void logNodeName(std::ostream & stream) const override {
    stream << "ConfidenceInterval";
  }
#endif

  // ExpressionNode

  // Properties
  Type type() const override { return Type::ConfidenceInterval; }
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
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
  static ConfidenceInterval Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<ConfidenceInterval, ConfidenceIntervalNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("confidence", 2, &UntypedBuilderTwoChildren<ConfidenceInterval>);

  // Expression
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
private:
  constexpr static int k_maxNValue = 300;
};

class SimplePredictionInterval final : public ConfidenceInterval {
public:
  SimplePredictionInterval(const SimplePredictionIntervalNode * n) : ConfidenceInterval(n) {}
  static SimplePredictionInterval Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<SimplePredictionInterval, SimplePredictionIntervalNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("prediction", 2, &UntypedBuilderTwoChildren<SimplePredictionInterval>);
};

}

#endif
