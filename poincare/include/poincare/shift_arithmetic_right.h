#ifndef POINCARE_SHIFT_ARITHMETIC_RIGHT_H
#define POINCARE_SHIFT_ARITHMETIC_RIGHT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class ShiftArithmeticRightNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(ShiftArithmeticRightNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "ShiftArithmeticRight";
    }
#endif

    // Properties
    Type type() const override { return Type::ShiftArithmeticRight; }

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

  class ShiftArithmeticRight final : public Expression
  {
  public:
    ShiftArithmeticRight(const ShiftArithmeticRightNode *n) : Expression(n) {}
    static ShiftArithmeticRight Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<ShiftArithmeticRight, ShiftArithmeticRightNode>({child1, child2}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sra", 2, &UntypedBuilderTwoChildren<ShiftArithmeticRight>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
