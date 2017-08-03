#ifndef POINCARE_DIVISION_REMAINDER_H
#define POINCARE_DIVISION_REMAINDER_H

#include <poincare/function.h>

namespace Poincare {

class DivisionRemainder : public Function {
public:
  DivisionRemainder();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context & context, AngleUnit angleUnit) const override;
};

}

#endif

