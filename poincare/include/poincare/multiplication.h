#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Multiplication : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
      int numnerOfOperands, bool cloneOperands = true) const override;
  template<typename T> static Evaluation<T> * computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n);
  template<typename T> static Evaluation<T> * computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * m);
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;

   Evaluation<float> * computeOnComplexMatrices(Evaluation<float> * m, Evaluation<float> * n) const override {
    return computeOnMatrices(m, n);
  }
  Evaluation<double> * computeOnComplexMatrices(Evaluation<double> * m, Evaluation<double> * n) const override {
    return computeOnMatrices(m, n);
  }

  Complex<float> privateCompute(const Complex<float> c, const Complex<float> d) const override {
    return compute(c, d);
  }
  Complex<double> privateCompute(const Complex<double> c, const Complex<double> d) const override {
    return compute(c, d);
  }
};

}

#endif
