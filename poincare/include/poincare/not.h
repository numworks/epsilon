#ifndef POINCARE_NOT_H
#define POINCARE_NOT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare
{

  class NotNode final : public ExpressionNode
  {
  public:
    // TreeNode
    size_t size() const override { return sizeof(NotNode); }
    int numberOfChildren() const override;
#if POINCARE_TREE_LOG
    void logNodeName(std::ostream &stream) const override
    {
      stream << "Not";
    }
#endif

    // Properties
    Type type() const override { return Type::Not; }

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

  class Not final : public Expression
  {
  public:
    Not(const NotNode *n) : Expression(n) {}
    static Not Builder(Expression child1) { return TreeHandle::FixedArityBuilder<Not, NotNode>({child1}); }

    static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("not", 1, &UntypedBuilderOneChild<Not>);

    Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
  };

} // namespace Poincare

#endif
