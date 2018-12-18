#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/expression.h>
#include <poincare/symbol.h>
#include <poincare/variable_context.h>

namespace Poincare {

class DerivativeNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(DerivativeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Derivative";
  }
#endif

  // Properties
  Type type() const override { return Type::Derivative; }
  int polynomialDegree(Context & context, const char * symbolName) const override;
  Expression replaceUnknown(const Symbol & symbol) override;

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
  template<typename T> T approximateWithArgument(T x, Context & context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T growthRateAroundAbscissa(T x, T h, Context & context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T riddersApproximation(Context & context, Preferences::AngleUnit angleUnit, T x, T h, T * error) const;
  // TODO: Change coefficients?
  constexpr static double k_maxErrorRateOnApproximation = 0.001;
  constexpr static double k_minInitialRate = 0.01;
  constexpr static double k_rateStepSize = 1.4;
};

class Derivative final : public Expression {
public:
  Derivative(const DerivativeNode * n) : Expression(n) {}
  static Derivative Builder(Expression child0, Symbol child1, Expression child2) { return Derivative(child0, child1, child2); }
  static Expression UntypedBuilder(Expression children) {
    if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
      // Second parameter must be a Symbol.
      return Expression();
    }
    return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2));
  }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("diff", 3, &UntypedBuilder);

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  Derivative(Expression child0, Expression child1, Expression child2) : Expression(TreePool::sharedPool()->createTreeNode<DerivativeNode>()) {
    assert(child1.type() == ExpressionNode::Type::Symbol);
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
    replaceChildAtIndexInPlace(2, child2);
  }
};

}

#endif
