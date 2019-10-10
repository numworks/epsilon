#include <poincare/approximation_helper.h>
#include <poincare/expression.h>
#include <poincare/evaluation.h>
#include <poincare/matrix_complex.h>
#include <cmath>
#include <utility>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template <typename T> T absMod(T a, T b) {
  T result = std::fmod(std::fabs(a), b);
  return result > b/2 ? b-result : result;
}

template <typename T> std::complex<T> ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument(std::complex<T> c) {
  T arg = std::arg(c);
  T precision = 10*Expression::Epsilon<T>();
  if (absMod<T>(arg, (T)M_PI) <= precision) {
    c.imag(0);
  }
  if (absMod<T>(arg-(T)M_PI/2.0, (T)M_PI) <= precision) {
    c.real(0);
  }
  return c;
}

template<typename T> Evaluation<T> ApproximationHelper::Map(const ExpressionNode * expression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute) {
  assert(expression->numberOfChildren() == 1);
  Evaluation<T> input = expression->childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  if (input.type() == EvaluationNode<T>::Type::Complex) {
    return compute(static_cast<Complex<T> &>(input).stdComplex(), complexFormat, angleUnit);
  } else {
    assert(input.type() == EvaluationNode<T>::Type::MatrixComplex);
    MatrixComplex<T> m = static_cast<MatrixComplex<T> &>(input);
    MatrixComplex<T> result = MatrixComplex<T>::Builder();
    for (int i = 0; i < m.numberOfChildren(); i++) {
      result.addChildAtIndexInPlace(compute(m.complexAtIndex(i), complexFormat, angleUnit), i, i);
    }
    result.setDimensions(m.numberOfRows(), m.numberOfColumns());
    return std::move(result);
  }
}

template<typename T> Evaluation<T> ApproximationHelper::MapReduce(const ExpressionNode * expression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices) {
  assert(expression->numberOfChildren() > 0);
  Evaluation<T> result = expression->childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  for (int i = 1; i < expression->numberOfChildren(); i++) {
    Evaluation<T> intermediateResult;
    Evaluation<T> nextOperandEvaluation = expression->childAtIndex(i)->approximate(T(), context, complexFormat, angleUnit);
    if (result.type() == EvaluationNode<T>::Type::Complex && nextOperandEvaluation.type() == EvaluationNode<T>::Type::Complex) {
      intermediateResult = computeOnComplexes(static_cast<Complex<T> &>(result).stdComplex(), static_cast<Complex<T> &>(nextOperandEvaluation).stdComplex(), complexFormat);
    } else if (result.type() == EvaluationNode<T>::Type::Complex) {
      assert(nextOperandEvaluation.type() == EvaluationNode<T>::Type::MatrixComplex);
      intermediateResult = computeOnComplexAndMatrix(static_cast<Complex<T> &>(result).stdComplex(), static_cast<MatrixComplex<T> &>(nextOperandEvaluation), complexFormat);
    } else if (nextOperandEvaluation.type() == EvaluationNode<T>::Type::Complex) {
      assert(result.type() == EvaluationNode<T>::Type::MatrixComplex);
      intermediateResult = computeOnMatrixAndComplex(static_cast<MatrixComplex<T> &>(result), static_cast<Complex<T> &>(nextOperandEvaluation).stdComplex(), complexFormat);
    } else {
      assert(result.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      assert(nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      intermediateResult = computeOnMatrices(static_cast<MatrixComplex<T> &>(result), static_cast<MatrixComplex<T> &>(nextOperandEvaluation), complexFormat);
    }
    result = intermediateResult;
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

template<typename T> MatrixComplex<T> ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Poincare::Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfChildren(); i++) {
    matrix.addChildAtIndexInPlace(computeOnComplexes(m.complexAtIndex(i), c, complexFormat), i, i);
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template<typename T> MatrixComplex<T> ApproximationHelper::ElementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Poincare::Preferences::ComplexFormat complexFormat, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.numberOfRows() != n.numberOfRows() || m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfChildren(); i++) {
    matrix.addChildAtIndexInPlace(computeOnComplexes(m.complexAtIndex(i), n.complexAtIndex(i), complexFormat), i, i);
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template std::complex<float> Poincare::ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument<float>(std::complex<float>);
template std::complex<double> Poincare::ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument<double>(std::complex<double>);
template Poincare::Evaluation<float> Poincare::ApproximationHelper::Map(const Poincare::ExpressionNode * expression, Poincare::Context * context, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationHelper::ComplexCompute<float> compute);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::Map(const Poincare::ExpressionNode * expression, Poincare::Context * context, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationHelper::ComplexCompute<double> compute);
template Poincare::Evaluation<float> Poincare::ApproximationHelper::MapReduce(const Poincare::ExpressionNode * expression, Poincare::Context * context, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationHelper::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationHelper::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationHelper::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationHelper::MatrixAndMatrixReduction<float> computeOnMatrices);
template Poincare::Evaluation<double> Poincare::ApproximationHelper::MapReduce(const Poincare::ExpressionNode * expression, Poincare::Context * context, Poincare::Preferences::ComplexFormat, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationHelper::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationHelper::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationHelper::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationHelper::MatrixAndMatrixReduction<double> computeOnMatrices);
template Poincare::MatrixComplex<float> Poincare::ApproximationHelper::ElementWiseOnMatrixComplexAndComplex<float>(const Poincare::MatrixComplex<float>, const std::complex<float>, Poincare::Preferences::ComplexFormat, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<double> Poincare::ApproximationHelper::ElementWiseOnMatrixComplexAndComplex<double>(const Poincare::MatrixComplex<double>, std::complex<double> const, Poincare::Preferences::ComplexFormat, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<float> Poincare::ApproximationHelper::ElementWiseOnComplexMatrices<float>(const Poincare::MatrixComplex<float>, const Poincare::MatrixComplex<float>, Poincare::Preferences::ComplexFormat, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>, Poincare::Preferences::ComplexFormat));
template Poincare::MatrixComplex<double> Poincare::ApproximationHelper::ElementWiseOnComplexMatrices<double>(const Poincare::MatrixComplex<double>, const Poincare::MatrixComplex<double>, Poincare::Preferences::ComplexFormat, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>, Poincare::Preferences::ComplexFormat));


}
