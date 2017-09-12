#ifndef POINCARE_N_ARY_OPERATION_H
#define POINCARE_N_ARY_OPERATION_H

#include <poincare/expression.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/complex_matrix.h>

namespace Poincare {

class NAryOperation : public Expression {
public:
  NAryOperation();
  NAryOperation(Expression ** operands, int numberOfOperands, bool cloneOperands = true);
  ~NAryOperation();
  NAryOperation(const NAryOperation& other) = delete;
  NAryOperation(NAryOperation&& other) = delete;
  NAryOperation& operator=(const NAryOperation& other) = delete;
  NAryOperation& operator=(NAryOperation&& other) = delete;
  bool hasValidNumberOfArguments() const override;
  const Expression * operand(int i) const override;
  int numberOfOperands() const override;
  Expression * clone() const override;

  void replaceChild(Expression * oldChild, Expression * newChild);
  void removeChild(Expression * oldChild);
  void stealOperandsFrom(NAryOperation * sibling);
protected:
  Expression ** m_operands;
  int m_numberOfOperands;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  virtual Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  virtual Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;

  virtual Evaluation<float> * computeOnComplexAndComplexMatrix(const Complex<float> * c, Evaluation<float> * n) const {
    return templatedComputeOnComplexAndComplexMatrix(c, n);
  }
  virtual Evaluation<double> * computeOnComplexAndComplexMatrix(const Complex<double> * c, Evaluation<double> * n) const {
    return templatedComputeOnComplexAndComplexMatrix(c, n);
  }
  template<typename T> Evaluation<T> * templatedComputeOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n) const;

  virtual Evaluation<float> * computeOnComplexMatrixAndComplex(Evaluation<float> * m, const Complex<float> * d) const {
    return templatedComputeOnComplexMatrixAndComplex(m, d);
  }
  virtual Evaluation<double> * computeOnComplexMatrixAndComplex(Evaluation<double> * m, const Complex<double> * d) const {
    return templatedComputeOnComplexMatrixAndComplex(m, d);
  }
  template<typename T> Evaluation<T> * templatedComputeOnComplexMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d) const;

  virtual Evaluation<float> * computeOnComplexMatrices(Evaluation<float> * m, Evaluation<float> * n) const {
    return templatedComputeOnComplexMatrices(m, n);
  }
  virtual Evaluation<double> * computeOnComplexMatrices(Evaluation<double> * m, Evaluation<double> * n) const {
    return templatedComputeOnComplexMatrices(m, n);
  }
  template<typename T> Evaluation<T> * templatedComputeOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n) const;

  virtual Complex<float> privateCompute(const Complex<float> c, const Complex<float> d) const = 0;
  virtual Complex<double> privateCompute(const Complex<double> c, const Complex<double> d) const = 0;
private:
  virtual char operatorChar() const;
};

}

#endif
