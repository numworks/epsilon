#ifndef POINCARE_RIGHTWARDS_ARROW_EXPRESSION_H
#define POINCARE_RIGHTWARDS_ARROW_EXPRESSION_H

#include <poincare/expression.h>
#include <poincare/symbol_abstract.h>
#include <poincare/evaluation.h>

namespace Poincare {

class RightwardsArrowExpressionNode : public ExpressionNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 2; }
  // ExpressionNode
  int polynomialDegree(Context * context, const char * symbolName) const override { return -1; }

private:
  // Simplification
  void deepReduceChildren(ExpressionNode::ReductionContext reductionContext) override {}
  LayoutShape leftLayoutShape() const override { assert(false); return LayoutShape::MoreLetters; };
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
};

}

#endif
