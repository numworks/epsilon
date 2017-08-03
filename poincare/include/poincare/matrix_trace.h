#ifndef POINCARE_MATRIX_TRACE_H
#define POINCARE_MATRIX_TRACE_H

#include <poincare/function.h>

namespace Poincare {

class MatrixTrace : public Function {
public:
  MatrixTrace();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
private:
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif

