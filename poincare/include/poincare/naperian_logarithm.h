#ifndef POINCARE_NAPERIAN_LOGARITHM_H
#define POINCARE_NAPERIAN_LOGARITHM_H

#include <poincare/function.h>

class NaperianLogarithm : public Function {
public:
  NaperianLogarithm();
  float approximate(Context & context, AngleUnit angleUnit = AngleUnit::Radian) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
};

#endif
