#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/rational.h>
#include <poincare/layout_engine.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class Addition : public DynamicHierarchy {
  using DynamicHierarchy::DynamicHierarchy;
  friend class Logarithm;
  friend class Multiplication;
  friend class Subtraction;
  friend class Power;
  friend class Complex<float>;
  friend class Complex<double>;
public:
  Type type() const override;
  Expression * clone() const override;
  int polynomialDegree(char symbolName) const override;
  /* Evaluation */
  template<typename T> static Complex<T> compute(const Complex<T> c, const Complex<T> d);
  template<typename T> static Matrix * computeOnMatrices(const Matrix * m, const Matrix * n) {
    return ApproximationEngine::elementWiseOnComplexMatrices(m, n, compute<T>);
  }
  template<typename T> static Matrix * computeOnComplexAndMatrix(const Complex<T> * c, const Matrix * m) {
    return ApproximationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
  }
private:
  /* Layout */
  bool needParenthesisWithParent(const Expression * e) const override;
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createInfixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  static const char * name() { return "+"; }

  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  Expression * shallowBeautify(Context & context, AngleUnit angleUnit) override;
  Expression * factorizeOnCommonDenominator(Context & context, AngleUnit angleUnit);
  void factorizeOperands(Expression * e1, Expression * e2, Context & context, AngleUnit angleUnit);
  static const Rational RationalFactor(Expression * e);
  static bool TermsHaveIdenticalNonRationalFactors(const Expression * e1, const Expression * e2);
  /* Evaluation */
  template<typename T> static Matrix * computeOnMatrixAndComplex(const Matrix * m, const Complex<T> * c) {
    return ApproximationEngine::elementWiseOnComplexAndComplexMatrix(c, m, compute<T>);
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
