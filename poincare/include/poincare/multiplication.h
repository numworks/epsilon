#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/dynamic_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class Multiplication : public DynamicHierarchy {
  using DynamicHierarchy::DynamicHierarchy;
  friend class Addition;
  friend class Division;
  friend class Logarithm;
  friend class Opposite;
  friend class Power;
  friend class Subtraction;
  friend class Symbol;
  friend class Complex<float>;
  friend class Complex<double>;
public:
  Type type() const override;
  Expression * clone() const override;
  Sign sign() const override;
  int polynomialDegree(char symbolName) const override;
  int privateGetPolynomialCoefficients(char symbolName, Expression * coefficients[]) const override;
  /* Evaluation */
  template<typename T> static std::complex<T> compute(const std::complex<T> c, const std::complex<T> d);
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
    return ApproximationEngine::elementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n);
private:
  /* Property */
  Expression * setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;
  /* Layout */
  bool needParenthesisWithParent(const Expression * e) const override;
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression * shallowReduce(Context& context, Preferences::AngleUnit angleUnit) override;
  Expression * privateShallowReduce(Context& context, Preferences::AngleUnit angleUnit, bool expand, bool canBeInterrupted);
  void mergeMultiplicationOperands();
  void factorizeBase(Expression * e1, Expression * e2, Context & context, Preferences::AngleUnit angleUnit);
  void factorizeExponent(Expression * e1, Expression * e2, Context & context, Preferences::AngleUnit angleUnit);
  Expression * distributeOnOperandAtIndex(int index, Context & context, Preferences::AngleUnit angleUnit);
  Expression * cloneDenominator(Context & context, Preferences::AngleUnit angleUnit) const override;
  void addMissingFactors(Expression * factor, Context & context, Preferences::AngleUnit angleUnit);
  void factorizeSineAndCosine(Expression * o1, Expression * o2, Context & context, Preferences::AngleUnit angleUnit);
  static bool HaveSameNonRationalFactors(const Expression * e1, const Expression * e2);
  static bool TermsHaveIdenticalBase(const Expression * e1, const Expression * e2);
  static bool TermsHaveIdenticalExponent(const Expression * e1, const Expression * e2);
  static bool TermHasRationalBase(const Expression * e);
  static bool TermHasRationalExponent(const Expression * e);
  static const Expression * CreateExponent(Expression * e);
  Expression * shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) override;
  // Warning: mergeNegativePower not always returns  a multiplication: *(b^-1,c^-1) -> (bc)^-1
  Expression * mergeNegativePower(Context & context, Preferences::AngleUnit angleUnit);
  /* Evaluation */

  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c) {
    return ApproximationEngine::elementWiseOnMatrixComplexAndComplex(m, c, compute<T>);
  }
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<float>(this, context, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationEngine::mapReduce<double>(this, context, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

}

#endif
