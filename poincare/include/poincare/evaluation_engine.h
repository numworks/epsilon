#ifndef POINCARE_EVALUATION_ENGINE_H
#define POINCARE_EVALUATION_ENGINE_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>
#include <poincare/complex.h>

namespace Poincare {

class EvaluationEngine {
public:
  template <typename T> using ExpressionToComplexMap = Complex<T>(*)(const Complex<T>, Expression::AngleUnit angleUnit);
  template<typename T> static Evaluation<T> * map(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ExpressionToComplexMap<T> compute);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const Complex<T>, const Complex<T>);
  template <typename T> using ComplexAndMatrixReduction = Evaluation<T> * (*)(const Complex<T> * c, Evaluation<T> * m);
  template <typename T> using MatrixAndComplexReduction = Evaluation<T> * (*)(Evaluation<T> * m, const Complex<T> * c);
  template <typename T> using MatrixAndMatrixReduction = Evaluation<T> * (*)(Evaluation<T> * m, Evaluation<T> * n);
  template<typename T> static Evaluation<T> * mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);


  template<typename T> static Evaluation<T> * elementWiseOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> static Evaluation<T> * elementWiseOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n, ComplexAndComplexReduction<T> computeOnComplexes);
};

}

#endif
