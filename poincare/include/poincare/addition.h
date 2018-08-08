#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <poincare/approximation_helper.h>

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
  int polynomialDegree(char symbolName) const override;
  int privateGetPolynomialCoefficients(char symbolName, Expression * coefficients[]) const override;
  /* Evaluation */
  template<typename T> static std::complex<T> compute(const std::complex<T> c, const std::complex<T> d);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
    return ApproximationHelper::ElementWiseOnComplexMatrices(m, n, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
private:
  /* Layout */
  bool needsParenthesesWithParent(SerializableNode * parentNode) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Infix(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::writeInfixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  static const char * name() { return "+"; }

  /* Simplification */
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  Expression * shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  Expression * factorizeOnCommonDenominator(Context & context, Preferences::AngleUnit angleUnit);
  void factorizeOperands(Expression * e1, Expression * e2, Context & context, Preferences::AngleUnit angleUnit);
  static const Rational RationalFactor(Expression * e);
  static bool TermsHaveIdenticalNonRationalFactors(const Expression * e1, const Expression * e2);
  /* Evaluation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
   }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
   }
};

}

#endif
