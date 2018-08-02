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

  template <typename T> using ComplexCompute = ComplexReference<T>(*)(const std::complex<T>, Preferences::AngleUnit angleUnit);
  template<typename T> static EvaluationReference<T> map(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = ComplexReference<T>(*)(const std::complex<T>, const std::complex<T>);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplexReference<T>(*)(const std::complex<T> c, const MatrixComplexReference<T> m);
  template <typename T> using MatrixAndComplexReduction = MatrixComplexReference<T>(*)(const MatrixComplexReference<T> m, const std::complex<T> c);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplexReference<T>(*)(const MatrixComplexReference<T> m, const MatrixComplexReference<T> n);
  template<typename T> static EvaluationReference<T> mapReduce(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);

  template<typename T> static MatrixComplexReference<T> elementWiseOnMatrixComplexAndComplex(const MatrixComplexReference<T> n, std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> static MatrixComplexReference<T> elementWiseOnComplexMatrices(const MatrixComplexReference<T> m, const MatrixComplexReference<T> n, ComplexAndComplexReduction<T> computeOnComplexes);

};

}

#endif
