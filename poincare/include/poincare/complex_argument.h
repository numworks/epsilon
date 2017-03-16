#ifndef POINCARE_COMPLEX_ARGUMENT_H
#define POINCARE_COMPLEX_ARGUMENT_H

#include <poincare/function.h>

namespace Poincare {

class ComplexArgument : public Function {
public:
  ComplexArgument();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
};

}

#endif

