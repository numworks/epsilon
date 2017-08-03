#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <poincare/matrix.h>

namespace Poincare {

class Complex;

class Evaluation : public Matrix {
public:
  virtual float toFloat() const = 0;
  Type type() const override;
  bool hasValidNumberOfArguments() const override;
  virtual const Expression * operand(int i) const override;
  virtual const Complex * complexOperand(int i) const = 0;
  virtual Evaluation * clone() const override = 0;
  virtual Evaluation * createTrace() const;
  virtual Evaluation * createDeterminant() const;
  virtual Evaluation * createInverse() const;
  Evaluation * createTranspose() const;
private:
  Evaluation * privateEvaluate(Context& context, AngleUnit angleUnit) const override;
};

}

#endif
