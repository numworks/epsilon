#ifndef POINCARE_SHIFT_LOGIC_LEFT_H
#define POINCARE_SHIFT_LOGIC_LEFT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class ShiftLogicLeftNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(ShiftLogicLeftNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "ShiftLogicLeft";
    }
#endif

    // Properties
    Type type() const override { return Type::ShiftLogicLeft; }

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

  class ShiftLogicLeft final : public Expression
  {
  public:
    ShiftLogicLeft(const ShiftLogicLeftNode *n) : Expression(n) {}
    static ShiftLogicLeft Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<ShiftLogicLeft, ShiftLogicLeftNode>({child1, child2}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sll", 2, &UntypedBuilderTwoChildren<ShiftLogicLeft>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
