#ifndef POINCARE_MATRIX_TRACE_H
#define POINCARE_MATRIX_TRACE_H

#include "old_expression.h"

namespace Poincare {

class MatrixTraceNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "trace";

  // PoolObject
  size_t size() const override { return sizeof(MatrixTraceNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "MatrixTrace";
  }
#endif

  // Properties
  Type otype() const override { return Type::MatrixTrace; }

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
};

class MatrixTrace final
    : public ExpressionOneChild<MatrixTrace, MatrixTraceNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
