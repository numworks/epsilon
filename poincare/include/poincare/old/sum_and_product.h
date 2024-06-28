#ifndef POINCARE_SUM_AND_PRODUCT_H
#define POINCARE_SUM_AND_PRODUCT_H

#include "approximation_helper.h"
#include "parametered_expression.h"
#include "symbol.h"

namespace Poincare {

class SumAndProductNode : public ParameteredExpressionNode {
 public:
  int numberOfChildren() const override { return 4; }

 private:
  // Simplication
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  };
};

class SumAndProduct : public OExpression {
 public:
  SumAndProduct(const SumAndProductNode* n) : OExpression(n) {}
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
