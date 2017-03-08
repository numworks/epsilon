#ifndef POINCARE_IMAGINARY_PART_H
#define POINCARE_IMAGINARY_PART_H

#include <poincare/function.h>

namespace Poincare {

class ImaginaryPart : public Function {
public:
  ImaginaryPart();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  float privateApproximate(Context & context, AngleUnit angleUnit) const override;
};

}

#endif


