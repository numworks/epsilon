#ifndef POINCARE_DIVISION_H
#define POINCARE_DIVISION_H

#include <poincare/static_hierarchy.h>
#include <poincare/approximation_engine.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class Division : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
  friend class Multiplication;
  friend class Symbol;
  friend class Power;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
private:
  /* Layout */
  bool operandNeedParenthesis(const Expression * e) const override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, "/");
  }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static Matrix * computeOnMatrixAndComplex(const Matrix * m, const Complex<T> * c) {
    return ApproximationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Matrix * computeOnComplexAndMatrix(const Complex<T> * c, const Matrix * n);
  template<typename T> static Matrix * computeOnMatrices(const Matrix * m, const Matrix * n);

  virtual Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  virtual Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

}

#endif
