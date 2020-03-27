#ifndef POINCARE_APPROXIMATION_HELPER_H
#define POINCARE_APPROXIMATION_HELPER_H

#include <poincare/complex.h>
#include <poincare/matrix_complex.h>
#include <poincare/expression_node.h>
#include <complex.h>

namespace Poincare {

namespace ApproximationHelper {
  template <typename T> int PositiveIntegerApproximationIfPossible(const ExpressionNode * expression, bool * isUndefined, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  template <typename T> std::complex<T> NeglectRealOrImaginaryPartIfNeglectable(std::complex<T> result, std::complex<T> input1, std::complex<T> input2 = 1.0, bool enableNullResult = true);

  template <typename T> using ComplexCompute = Complex<T>(*)(const std::complex<T>, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit);
  template<typename T> Evaluation<T> Map(const ExpressionNode * expression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const std::complex<T>, const std::complex<T>, Preferences::ComplexFormat complexFormat);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplex<T>(*)(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat);
  template <typename T> using MatrixAndComplexReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  template<typename T> Evaluation<T> MapReduce(const ExpressionNode * expression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);

  template<typename T> MatrixComplex<T> ElementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> n, std::complex<T> c, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> MatrixComplex<T> ElementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes);
};

}

#endif
