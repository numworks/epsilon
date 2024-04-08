#ifndef POINCARE_MATRIX_TRANSPOSE_H
#define POINCARE_MATRIX_TRANSPOSE_H

#include "old_expression.h"

namespace Poincare {

class MatrixTransposeNode final : public ExpressionNode {
 public:
  constexpr static AliasesList k_functionName = "transpose";

  // PoolObject
  size_t size() const override { return sizeof(MatrixTransposeNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "MatrixTranspose";
  }
#endif

  // Properties
  Type otype() const override { return Type::MatrixTranspose; }

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
};

class MatrixTranspose final
    : public ExpressionOneChild<MatrixTranspose, MatrixTransposeNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder;
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
