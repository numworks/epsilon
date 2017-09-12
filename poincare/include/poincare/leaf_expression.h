#ifndef POINCARE_LEAF_EXPRESSION_H
#define POINCARE_LEAF_EXPRESSION_H

#include <poincare/expression.h>

namespace Poincare {

class LeafExpression : public Expression {
public:
  bool hasValidNumberOfArguments() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
  void sort() override;
private:
  bool nodeEquals(const Expression * e) const override;
  virtual bool valueEquals(const Expression * e) const = 0;
  bool nodeGreaterThan(const Expression * e) const override;
  virtual bool valueGreaterThan(const Expression * e) const = 0;
};

}

#endif
