#ifndef POINCARE_RIGHTWARDS_ARROW_EXPRESSION_H
#define POINCARE_RIGHTWARDS_ARROW_EXPRESSION_H

#include <poincare/evaluation.h>
#include <poincare/expression.h>
#include <poincare/symbol_abstract.h>

namespace Poincare {

class RightwardsArrowExpressionNode : public ExpressionNode {
 public:
  // TreeNode
  int numberOfChildren() const override { return 2; }
  // ExpressionNode
  int polynomialDegree(Context* context,
                       const char* symbolName) const override {
    return -1;
  }

 private:
  // Simplification
  LayoutShape leftLayoutShape() const override {
    assert(false);
    return LayoutShape::MoreLetters;
  };
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
};

}  // namespace Poincare

#endif
