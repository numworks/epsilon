#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/function.h>
#include <poincare/n_context.h>

class Sum : public Function {
public:
  Sum();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
  ExpressionLayout * createLayout() const override;
};

#endif
