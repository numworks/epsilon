#ifndef POINCARE_NORMAL_DISTRIBUTION_FUNCTION_H
#define POINCARE_NORMAL_DISTRIBUTION_FUNCTION_H

#include <poincare/expression.h>

namespace Poincare {

// NormalDistributionFunctions are NormCDF, NormCDFRange, InvNorm and NormPDF

class NormalDistributionFunctionNode : public ExpressionNode {
public:
  virtual int muIndex() const { return 1; }
  virtual int varIndex() const { return 2; }
private:
  // Simplication
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class NormalDistributionFunction : public Expression {
public:
  NormalDistributionFunction(const NormalDistributionFunctionNode * n) : Expression(n) {}
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext, bool * stopReduction = nullptr);
private:
  int muIndex() const { return node()->muIndex(); }
  int varIndex() const { return node()->varIndex(); }
  NormalDistributionFunctionNode * node() const { return static_cast<NormalDistributionFunctionNode *>(Expression::node()); }
};

}

#endif
