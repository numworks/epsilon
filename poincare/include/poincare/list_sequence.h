#ifndef POINCARE_LIST_SEQUENCE_H
#define POINCARE_LIST_SEQUENCE_H

#include <poincare/parametered_expression.h>
#include <poincare/complex.h>
#include <poincare/list_complex.h>
#include <poincare/symbol.h>

namespace Poincare {

class ListSequenceNode final : public ParameteredExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(ListSequenceNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "ListSequence";
  }
#endif
   // Properties
  Type type() const override { return Type::ListSequence; }

private:
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;

  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }

  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

};

class ListSequence : public ParameteredExpression {
public:
  ListSequence(const ListSequenceNode * n) : ParameteredExpression(n) {}
  static ListSequence Builder(Expression function, Symbol variable, Expression variableUpperBound) { return TreeHandle::FixedArityBuilder<ListSequence, ListSequenceNode>({function, variable, variableUpperBound}); }
  static Expression UntypedBuilder(Expression children);
  constexpr static Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sequence", 3, &UntypedBuilder);
  constexpr static char k_defaultXNTChar = 'k';

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);

};

}


#endif
