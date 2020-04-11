#ifndef POINCARE_MATRIX_IDENTITY_H
#define POINCARE_MATRIX_IDENTITY_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixIdentityNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(MatrixIdentityNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixIdentity";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixIdentity; }
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

class MatrixIdentity final : public Expression {
public:
  MatrixIdentity(const MatrixIdentityNode * n) : Expression(n) {}
  static MatrixIdentity Builder(Expression child) { return TreeHandle::FixedArityBuilder<MatrixIdentity, MatrixIdentityNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("identity", 1, &UntypedBuilderOneChild<MatrixIdentity>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
