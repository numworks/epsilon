#ifndef POINCARE_LEAF_EXPRESSION_H
#define POINCARE_LEAF_EXPRESSION_H

#include <poincare/expression.h>

class LeafExpression : public Expression {
  public:
    Expression * operand(int i) override;
    int numberOfOperands() override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) override;
};

#endif
