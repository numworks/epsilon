#ifndef POINCARE_BIT_GET_H
#define POINCARE_BIT_GET_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class BitGetNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(BitGetNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "BitGet";
    }
#endif

    // Properties
    Type type() const override { return Type::BitGet; }

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

  class BitGet final : public Expression
  {
  public:
    BitGet(const BitGetNode *n) : Expression(n) {}
    static BitGet Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<BitGet, BitGetNode>({child1, child2}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("bit", 2, &UntypedBuilderTwoChildren<BitGet>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
