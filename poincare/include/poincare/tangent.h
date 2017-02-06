#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/function.h>

namespace Poincare {

class Tangent : public Function {
public:
  Tangent();
  float approximate(Context & context, AngleUnit angleUnit) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
  Expression * evaluate(Context& context, AngleUnit angleUnit = AngleUnit::Radian) const override;
};

}

#endif
