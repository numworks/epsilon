#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/function.h>

class Logarithm : public Function {
public:
  Logarithm();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
};

#endif
