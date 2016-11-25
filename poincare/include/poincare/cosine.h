#ifndef POINCARE_COSINE_H
#define POINCARE_COSINE_H

#include <poincare/function.h>

class Cosine : public Function {
public:
  Cosine();
  float approximate(Context & context) const override;
  Expression * createEvaluation(Context& context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
    int numberOfOperands, bool cloneOperands = true) const override;
};

#endif
