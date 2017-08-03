#ifndef POINCARE_CEILING_H
#define POINCARE_CEILING_H

#include <poincare/function.h>

namespace Poincare {

class Ceiling : public Function {
public:
  Ceiling();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
};

}

#endif


