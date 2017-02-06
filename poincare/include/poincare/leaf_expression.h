#ifndef POINCARE_LEAF_EXPRESSION_H
#define POINCARE_LEAF_EXPRESSION_H

#include <poincare/expression.h>

namespace Poincare {

class LeafExpression : public Expression {
  public:
    const Expression * operand(int i) const override;
    int numberOfOperands() const override;
    Expression * cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands = true) const override;
};

}

#endif
