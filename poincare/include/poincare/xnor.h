#ifndef POINCARE_XNOR_H
#define POINCARE_XNOR_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare
{

  class XnorNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(XnorNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "Xnor";
    }
#endif

    // Properties
    Type type() const override { return Type::Xnor; }

  private:
    // Layout
    Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
    int serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
    // Simplification
    Expression shallowReduce(ReductionContext reductionContext) override;
    LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
    LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
    // Evaluation
    Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override
    {
      return templatedApproximate<float>(approximationContext);
    }
    Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override
    {
      return templatedApproximate<double>(approximationContext);
    }
    template <typename T>
    Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
  };

  class Xnor final : public Expression
  {
  public:
    Xnor(const XnorNode *n) : Expression(n) {}
    static Xnor Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Xnor, XnorNode>({child1, child2}); }
    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("xnor", 2, &UntypedBuilderTwoChildren<Xnor>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
    bool derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue);
  };

} // namespace Poincare

#endif
