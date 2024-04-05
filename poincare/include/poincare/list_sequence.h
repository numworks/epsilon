#ifndef POINCARE_LIST_SEQUENCE_H
#define POINCARE_LIST_SEQUENCE_H

#include <poincare/complex.h>
#include <poincare/list_complex.h>
#include <poincare/parametered_expression.h>
#include <poincare/symbol.h>

namespace Poincare {

class ListSequenceNode final : public ParameteredExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(ListSequenceNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ListSequence";
  }
#endif
  // Properties
  Type otype() const override { return Type::ListSequence; }

 private:
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  // Layout
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

class ListSequence : public ParameteredExpression {
 public:
  ListSequence(const ListSequenceNode* n) : ParameteredExpression(n) {}
  static ListSequence Builder(OExpression function, Symbol variable,
                              OExpression variableUpperBound) {
    return TreeHandle::FixedArityBuilder<ListSequence, ListSequenceNode>(
        {function, variable, variableUpperBound});
  }
  static OExpression UntypedBuilder(OExpression children);
  constexpr static OExpression::FunctionHelper s_functionHelper =
      OExpression::FunctionHelper("sequence", 3, &UntypedBuilder);

  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
