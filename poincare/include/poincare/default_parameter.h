#ifndef POINCARE_DEFAULT_PARAMETER_H
#define POINCARE_DEFAULT_PARAMETER_H

#include <poincare/expression.h>

namespace Poincare {

class DefaultParameterNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(DefaultParameterNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "DefaultParameter";
  }
#endif

  // Properties
  Type type() const override { return Type::DefaultParameter; }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return 0; }

  // Simplification
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::OneLetter; };

  Expression actualExpression() const { assert(parent()); return static_cast<ExpressionNode *>(parent())->defaultParameterAtIndex(parent()->indexOfChild(this)); }

  // TODO: Remove when serialization is only on Layouts
  bool skipAtSerialization() const override { return true; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override { assert(false); return Layout(); }
  // Simplication
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const { return actualExpression().node()->approximate(T(), approximationContext); }
};

class DefaultParameter final : public Expression {
public:
  DefaultParameter(const DefaultParameterNode * n) : Expression(n) {}
  static DefaultParameter Builder() { return TreeHandle::FixedArityBuilder<DefaultParameter, DefaultParameterNode>(); }
  Expression actualExpression() { return static_cast<DefaultParameterNode *>(node())->actualExpression(); }
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);

};

}

#endif
