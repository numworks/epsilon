#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class Subtraction : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  int polynomialDegree(char symbolName) const override;
private:
  /* Layout */
  bool needParenthesisWithParent(const Expression * e) const override;
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createInfixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  static const char * name() { return "-"; }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static Matrix * computeOnMatrixAndComplex(const Matrix * m, const Complex<T> * c) {
    return ApproximationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
  template<typename T> static Matrix * computeOnComplexAndMatrix(const Complex<T> * c, const Matrix * n);
  template<typename T> static Matrix * computeOnMatrices(const Matrix * m, const Matrix * n) {
    return ApproximationEngine::elementWiseOnComplexMatrices(m, n, compute<T>);
  }

  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

}

#endif
