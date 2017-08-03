#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Subtraction : public BinaryOperation {
  using BinaryOperation::BinaryOperation;
public:
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression** newOperands,
    int numnerOfOperands, bool cloneOperands = true) const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;

  Evaluation<float> * computeOnComplexAndComplexMatrix(const Complex<float> * c, Evaluation<float> * n) const override {
    return templatedComputeOnComplexAndComplexMatrix(c, n);
  }
  Evaluation<double> * computeOnComplexAndComplexMatrix(const Complex<double> * c, Evaluation<double> * n) const override {
    return templatedComputeOnComplexAndComplexMatrix(c, n);
  }
  template<typename T> Evaluation<T> * templatedComputeOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n) const;

  Complex<float> privateCompute(const Complex<float> c, const Complex<float> d) const override {
    return compute(c, d);
  }
  Complex<double> privateCompute(const Complex<double> c, const Complex<double> d) const override {
    return compute(c, d);
  }
};

}

#endif
