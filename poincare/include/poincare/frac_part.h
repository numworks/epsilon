#ifndef POINCARE_FRAC_PART_H
#define POINCARE_FRAC_PART_H

#include <poincare/function.h>

namespace Poincare {

class FracPart : public Function {
public:
  FracPart();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
};

}

#endif


