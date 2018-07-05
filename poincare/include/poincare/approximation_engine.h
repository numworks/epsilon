#ifndef POINCARE_APPROXIMATION_ENGINE_H
#define POINCARE_APPROXIMATION_ENGINE_H

#include <poincare/evaluation.h>
#include <poincare/expression.h>
#include <complex>

namespace Poincare {

class ApproximationEngine {
public:
  template <typename T> using ComplexCompute = std::complex<T>(*)(const std::complex<T>, Expression::AngleUnit angleUnit);
  template<typename T> static Evaluation<T> * map(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = std::complex<T>(*)(const std::complex<T>, const std::complex<T>);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplex<T>(*)(const std::complex<T> c, const MatrixComplex<T> m);
  template <typename T> using MatrixAndComplexReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const std::complex<T> c);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const MatrixComplex<T> n);
  template<typename T> static Evaluation<T> * mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);

  template<typename T> static MatrixComplex<T> elementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> n, const std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> static MatrixComplex<T> elementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, ComplexAndComplexReduction<T> computeOnComplexes);

};

}

#endif
