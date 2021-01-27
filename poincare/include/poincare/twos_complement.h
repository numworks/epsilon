#ifndef POINCARE_TWOS_COMPLEMENT_H
#define POINCARE_TWOS_COMPLEMENT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class TwosComplementNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(TwosComplementNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "TwosComplement";
    }
#endif

    // Properties
    Type type() const override { return Type::TwosComplement; }

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

  class TwosComplement final : public Expression
  {
  public:
    TwosComplement(const TwosComplementNode *n) : Expression(n) {}
    static TwosComplement Builder(Expression child1, Expression child2) { return TreeHandle::FixedArityBuilder<TwosComplement, TwosComplementNode>({child1, child2}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("tc", 2, &UntypedBuilderTwoChildren<TwosComplement>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
