#ifndef POINCARE_MATRIX_TRACE_H
#define POINCARE_MATRIX_TRACE_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixTraceNode /*final*/ : public ExpressionNode {
public:
  static constexpr char k_functionName[] = "trace";

  // TreeNode
  size_t size() const override { return sizeof(MatrixTraceNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "MatrixTrace";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixTrace; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
 template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;
};

class MatrixTrace final : public ExpressionOneChild<MatrixTrace, MatrixTraceNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
};

}

#endif
