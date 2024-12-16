#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include "symbol_abstract.h"

namespace Poincare {

class SequenceNode final : public SymbolAbstractNode {
 public:
  using SymbolAbstractNode::SymbolAbstractNode;

  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Sequence";
  }
#endif

  Type otype() const override { return Type::Sequence; }
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;

 private:
  size_t nodeSize() const override { return sizeof(SequenceNode); }
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }

  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override;
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override;
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class Sequence final : public SymbolAbstract {
  friend SequenceNode;

 public:
  Sequence(const SequenceNode* n) : SymbolAbstract(n) {}

  // Simplification
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
