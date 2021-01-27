#ifndef POINCARE_AND_EXPLICIT_H
#define POINCARE_AND_EXPLICIT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class AndExplicitNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(AndExplicitNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "AndExplicit";
    }
#endif

    // Properties
    Type type() const override { return Type::AndExplicit; }

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

  class AndExplicit final : public Expression
  {
  public:
    AndExplicit(const AndExplicitNode *n) : Expression(n) {}
    static AndExplicit Builder(Expression child1, Expression child2, Expression child3) { return TreeHandle::FixedArityBuilder<AndExplicit, AndExplicitNode>({child1, child2, child3}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("and", 3, &UntypedBuilderThreeChildren<AndExplicit>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
