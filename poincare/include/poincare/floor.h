#ifndef POINCARE_FLOOR_H
#define POINCARE_FLOOR_H

#include <poincare/function.h>

namespace Poincare {

class Floor : public Function {
public:
  Floor();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
};

}

#endif


