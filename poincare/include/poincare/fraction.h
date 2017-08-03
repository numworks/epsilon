#ifndef POINCARE_FRACTION_H
#define POINCARE_FRACTION_H

#include <poincare/binary_operation.h>

namespace Poincare {

class Fraction : public BinaryOperation {
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

   Evaluation<float> * computeOnComplexMatrices(Evaluation<float> * m, Evaluation<float> * n) const override {
    return templatedComputeOnComplexMatrices(m, n);
  }
  Evaluation<double> * computeOnComplexMatrices(Evaluation<double> * m, Evaluation<double> * n) const override {
    return templatedComputeOnComplexMatrices(m, n);
  }
  template<typename T> Evaluation<T> * templatedComputeOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n) const;

  Complex<float> privateCompute(const Complex<float> c, const Complex<float> d) const override {
    return compute(c, d);
  }
  Complex<double> privateCompute(const Complex<double> c, const Complex<double> d) const override {
    return compute(c, d);
  }
};

}

#endif
