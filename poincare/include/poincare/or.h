#ifndef POINCARE_OR_H
#define POINCARE_OR_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class OrNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(OrNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "Or";
    }
#endif

    // Properties
    Type type() const override { return Type::Or; }

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

  class Or final : public Expression
  {
  public:
    Or(const OrNode *n) : Expression(n) {}
    static Or Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<Or, OrNode>({child1, child2}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("or", 2, &UntypedBuilderTwoChildren<Or>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
