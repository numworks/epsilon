#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/function.h>

namespace Poincare {

class Tangent : public Function {
public:
  Tangent();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
  Expression * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif
