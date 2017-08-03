#ifndef POINCARE_MATRIX_INVERSE_H
#define POINCARE_MATRIX_INVERSE_H

#include <poincare/function.h>

namespace Poincare {

class MatrixInverse : public Function {
public:
  MatrixInverse();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif

