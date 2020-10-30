#ifndef POINCARE_KMAT_H
#define POINCARE_KMAT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class KMatNode final : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(KMatNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "KMat";
  }
#endif

  // Properties
  Type type() const override { return Type::KMat; }

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  //Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return Complex<float>::RealUndefined(); }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override { return Complex<double>::RealUndefined(); }
};

class KMat final : public Expression {
public:
  KMat(const KMatNode * n) : Expression(n) {}
  static KMat Builder(Expression child0, Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<KMat, KMatNode>({child0, child1, child2}); }
  

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("kmat", 3, &UntypedBuilderThreeChildren<KMat>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

};

}

#endif
