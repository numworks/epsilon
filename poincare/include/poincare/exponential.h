#ifndef POINCARE_EXPONENTIAL_H
#define POINCARE_EXPONENTIAL_H

#include <poincare/function.h>

class Exponential : public Function {
public:
  Exponential();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
};

#endif
