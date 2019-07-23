#ifndef POINCARE_MULTIPLICATION_H
#define POINCARE_MULTIPLICATION_H

#include <poincare/approximation_helper.h>
#include <poincare/n_ary_expression.h>

namespace Poincare {

class MultiplicationNode /*final*/ : public NAryExpressionNode {
public:
  using NAryExpressionNode::NAryExpressionNode;

  // Properties
  Sign sign(Context * context) const override;
  int polynomialDegree(Context * context, const char * symbolName) const override;
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const override;

  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) { return Complex<T>::Builder(c*d); }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);

protected:
  // Layout
  bool childNeedsParenthesis(const TreeNode * child) const override;

private:
  /* Approximation */
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(this, context, complexFormat, angleUnit, compute<float>, computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>, computeOnMatrices<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(this, context, complexFormat, angleUnit, compute<double>, computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>, computeOnMatrices<double>);
  }
};

class Multiplication : public NAryExpression {
public:
  Multiplication(const MultiplicationNode * n) : NAryExpression(n) {}

  template<typename T> static void computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);
  // Expression
  int getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const;
};

}

#endif
