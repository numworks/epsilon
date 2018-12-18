#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class IntegralNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(IntegralNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Integral";
  }
#endif

  // ExpressionNode
  Type type() const override { return Type::Integral; }
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
  template<typename T>
  struct DetailedResult
  {
    T integral;
    T absoluteError;
  };
  constexpr static int k_maxNumberOfIterations = 10;
#ifdef LAGRANGE_METHOD
  template<typename T> T lagrangeGaussQuadrature(T a, T b, Context & context, Preferences::AngleUnit angleUnit) const;
#else
  template<typename T> DetailedResult<T> kronrodGaussQuadrature(T a, T b, Context & context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T adaptiveQuadrature(T a, T b, T eps, int numberOfIterations, Context & context, Preferences::AngleUnit angleUnit) const;
#endif
  template<typename T> T functionValueAtAbscissa(T x, Context & xcontext, Preferences::AngleUnit angleUnit) const;
};

class Integral final : public Expression {
public:
  Integral(const IntegralNode * n) : Expression(n) {}
  static Integral Builder(Expression child0, Symbol child1, Expression child2, Expression child3) { return Integral(child0, child1, child2, child3); }
  static Expression UntypedBuilder(Expression children) {
    if (children.childAtIndex(1).type() != ExpressionNode::Type::Symbol) {
      // Second parameter must be a Symbol.
      return Expression();
    }
    return Builder(children.childAtIndex(0), children.childAtIndex(1).convert<Symbol>(), children.childAtIndex(2), children.childAtIndex(3));
  }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("int", 4, &UntypedBuilder);

  // Expression
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  Integral(Expression child0, Expression child1, Expression child2, Expression child3) : Expression(TreePool::sharedPool()->createTreeNode<IntegralNode>()) {
    assert(child1.type() == ExpressionNode::Type::Symbol);
    replaceChildAtIndexInPlace(0, child0);
    replaceChildAtIndexInPlace(1, child1);
    replaceChildAtIndexInPlace(2, child2);
    replaceChildAtIndexInPlace(3, child3);
  }
};

}

#endif
