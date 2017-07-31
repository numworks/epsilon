#ifndef POINCARE_MATRIX_DIMENSION_H
#define POINCARE_MATRIX_DIMENSION_H

#include <poincare/function.h>

namespace Poincare {

class MatrixDimension : public Function {
public:
  MatrixDimension();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif

