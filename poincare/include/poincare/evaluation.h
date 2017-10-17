#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <poincare/matrix.h>

namespace Poincare {

template<class T>
class Complex;

template<typename T>
class Evaluation : public Matrix {
public:
  virtual T toScalar() const = 0;
  virtual const Expression * operand(int i) const override;
  virtual const Complex<T> * complexOperand(int i) const = 0;
  virtual Evaluation<T> * clone() const override = 0;
  virtual Evaluation<T> * createTrace() const;
  virtual Evaluation<T> * createDeterminant() const;
  virtual Evaluation<T> * createInverse() const;
  Evaluation<T> * createTranspose() const;
};

}

#endif
