#ifndef POINCARE_APPROXIMATION_ENGINE_H
#define POINCARE_APPROXIMATION_ENGINE_H

#include <poincare/complex.h>
#include <poincare/matrix_complex.h>
#include <poincare/expression_node.h>
#include <complex>

namespace Poincare {

class ApproximationEngine {
public:
  template <typename T> static std::complex<T> truncateRealOrImaginaryPartAccordingToArgument(std::complex<T> c);

  template <typename T> using ComplexCompute = Complex<T>(*)(const std::complex<T>, Preferences::AngleUnit angleUnit);
  template<typename T> static Evaluation<T> map(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const std::complex<T>, const std::complex<T>);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplex<T>(*)(const std::complex<T> c, const MatrixComplex<T> m);
  template <typename T> using MatrixAndComplexReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const std::complex<T> c);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const MatrixComplex<T> n);
  template<typename T> static Evaluation<T> mapReduce(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);

  template<typename T> static MatrixComplex<T> elementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> n, std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> static MatrixComplex<T> elementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, ComplexAndComplexReduction<T> computeOnComplexes);

};

}

#endif
