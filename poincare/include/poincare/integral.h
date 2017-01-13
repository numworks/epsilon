#ifndef POINCARE_INTEGRAL_H
#define POINCARE_INTEGRAL_H

#include <poincare/function.h>
#include <poincare/x_context.h>

class Integral : public Function {
public:
  Integral();
  float approximate(Context & context) const override;
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  float functionValueAtAbscissa(float x, XContext xcontext) const;
};

#endif
