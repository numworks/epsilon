#ifndef POINCARE_BIT_CLEAR_H
#define POINCARE_BIT_CLEAR_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class BitClearNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(BitClearNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "BitClear";
    }
#endif

    // Properties
    Type type() const override { return Type::BitClear; }

private:
    // Layout
    Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
    int serialize(char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
    // Simplification
    Expression shallowReduce(ReductionContext reductionContext) override;
    LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
    LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

    // Evaluation
    Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
    Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
    template <typename T>
    Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const
    {
      return Complex<T>::RealUndefined();
    }
  };

  class BitClear final : public Expression
  {
  public:
    BitClear(const BitClearNode *n) : Expression(n) {}
    static BitClear Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitClear, BitClearNode>({child1, child2}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bclr", 2, &UntypedBuilderTwoChildren<BitClear>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
