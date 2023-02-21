#ifndef POINCARE_N_ARY_INFIX_EXPRESSION_H
#define POINCARE_N_ARY_INFIX_EXPRESSION_H

#include <poincare/n_ary_expression.h>

// NAryInfixExpressionNode are additions and multiplications

namespace Poincare {

class NAryInfixExpressionNode : public NAryExpressionNode {
 public:
  using NAryExpressionNode::NAryExpressionNode;
  // Properties
  bool childAtIndexNeedsUserParentheses(const Expression& child,
                                        int childIndex) const override;

 protected:
  // Order
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;
  int simplificationOrderGreaterType(const ExpressionNode* e, bool ascending,
                                     bool ignoreParentheses) const override;
};

}  // namespace Poincare

#endif
