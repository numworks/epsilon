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
  template<typename T> static std::complex<T> compute(const std::complex<T> c, const std::complex<T> d);
  int polynomialDegree(char symbolName) const override;
private:
  /* Layout */
  bool needParenthesisWithParent(const Expression * e) const override;
  ExpressionLayout * createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::createInfixLayout(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  static const char * name() { return "-"; }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c) {
    return ApproximationEngine::elementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
    return ApproximationEngine::elementWiseOnComplexMatrices(m, n, compute<T>);
  }

  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

}

#endif
