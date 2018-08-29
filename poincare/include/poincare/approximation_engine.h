#ifndef POINCARE_APPROXIMATION_ENGINE_H
#define POINCARE_APPROXIMATION_ENGINE_H

#include <poincare/expression.h>
#include <poincare/complex.h>
#include <poincare/matrix.h>

namespace Poincare {

class ApproximationEngine {
public:
  template <typename T> using ComplexCompute = Complex<T>(*)(const Complex<T>, Expression::AngleUnit angleUnit);
  template<typename T> static Expression * map(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const Complex<T>, const Complex<T>);
  template <typename T> using ComplexAndMatrixReduction = Matrix * (*)(const Complex<T> * c, const Matrix * m);
  template <typename T> using MatrixAndComplexReduction = Matrix * (*)(const Matrix * m, const Complex<T> * c);
  template <typename T> using MatrixAndMatrixReduction = Matrix * (*)(const Matrix * m, const Matrix * n);
  template<typename T> static Expression * mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);

  template<typename T> static Matrix * elementWiseOnComplexAndComplexMatrix(const Complex<T> * c, const Matrix * n, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> static Matrix * elementWiseOnComplexMatrices(const Matrix * m, const Matrix * n, ComplexAndComplexReduction<T> computeOnComplexes);

};

}

#endif
