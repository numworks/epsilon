#ifndef POINCARE_PARENTHESIS_H
#define POINCARE_PARENTHESIS_H

#include <poincare/expression.h>
#include <poincare/complex_cartesian.h>

namespace Poincare {

class ParenthesisNode /*final*/ : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(ParenthesisNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Parenthesis";
  }
#endif

  // Properties
  Type type() const override { return Type::Parenthesis; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  Expression getUnit() const override { assert(false); return ExpressionNode::getUnit(); }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; };

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, complexFormat, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, complexFormat, angleUnit); }
private:
 template<typename T> Evaluation<T> templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
};

class Parenthesis final : public Expression {
public:
  Parenthesis(const ParenthesisNode * n) : Expression(n) {}
  static Parenthesis Builder(Expression child) { return TreeHandle::FixedArityBuilder<Parenthesis, ParenthesisNode>({child}); }
  // Expression
  Expression shallowReduce();
};

}

#endif
