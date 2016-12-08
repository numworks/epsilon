#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/function.h>

class Tangent : public Function {
public:
  Tangent();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
};

#endif
