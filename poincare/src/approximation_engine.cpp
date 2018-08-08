#include <poincare/approximation_engine.h>
#include <poincare/evaluation.h>
#include <poincare/matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template <typename T> T absMod(T a, T b) {
  T result = std::fmod(std::fabs(a), b);
  return result > b/2 ? b-result : result;
}

template <typename T> std::complex<T> ApproximationEngine::truncateRealOrImaginaryPartAccordingToArgument(std::complex<T> c) {
  T arg = std::arg(c);
  T precision = 10*Expression::epsilon<T>();
  if (absMod<T>(arg, (T)M_PI) <= precision) {
    c.imag(0);
  }
  if (absMod<T>(arg-(T)M_PI/2.0, (T)M_PI) <= precision) {
    c.real(0);
  }
  return c;
}

template<typename T> Evaluation<T> ApproximationEngine::map(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute) {
  assert(expression->numberOfChildren() == 1);
  Evaluation<T> input = expression->childAtIndex(0)->approximate(T(), context, angleUnit);
  if (input.node()->type() == EvaluationNode<T>::Type::AllocationFailure) {
    return Evaluation<T>(EvaluationNode<T>::FailedAllocationStaticNode());
  } else if (input.node()->type() == EvaluationNode<T>::Type::Complex) {
    const ComplexNode<T> * c = static_cast<ComplexNode<T> *>(input.node());
    return compute(*c, angleUnit);
  } else {
    assert(input.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
    MatrixComplex<T> m = MatrixComplex<T>(input.node());
    MatrixComplex<T> result;
    for (int i = 0; i < result.numberOfRows()*result.numberOfColumns(); i++) {
      ComplexNode<T> * child = static_cast<MatrixComplexNode<T> *>(m.node())->childAtIndex(i);
      if (child) {
        result.addChildTreeAtIndex(compute(*child, angleUnit), i, i);
      } else {
        result.addChildTreeAtIndex(Complex<T>::Undefined(), i, i);
      }
    }
    result.setDimensions(m.numberOfRows(), m.numberOfColumns());
    return result;
  }
}

template<typename T> Evaluation<T> ApproximationEngine::mapReduce(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices) {
  Evaluation<T> result = expression->childAtIndex(0)->approximate(T(), context, angleUnit);
  for (int i = 1; i < expression->numberOfChildren(); i++) {
    Evaluation<T> intermediateResult(nullptr);
    Evaluation<T> nextOperandEvaluation = expression->childAtIndex(i)->approximate(T(), context, angleUnit);
    if (result.node()->type() == EvaluationNode<T>::Type::AllocationFailure || nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::AllocationFailure) {
      return Evaluation<T>(EvaluationNode<T>::FailedAllocationStaticNode());
    }
    if (result.node()->type() == EvaluationNode<T>::Type::Complex && nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::Complex) {
      const ComplexNode<T> * c = static_cast<const ComplexNode<T> *>(result.node());
      const ComplexNode<T> * d = static_cast<const ComplexNode<T> *>(nextOperandEvaluation.node());
      intermediateResult = computeOnComplexes(*c, *d);
    } else if (result.node()->type() == EvaluationNode<T>::Type::Complex) {
      assert(nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      const ComplexNode<T> * c = static_cast<const ComplexNode<T> *>(result.node());
      MatrixComplex<T> n = MatrixComplex<T>(nextOperandEvaluation.node());
      intermediateResult = computeOnComplexAndMatrix(*c, n);
    } else if (nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::Complex) {
      assert(result.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      MatrixComplex<T> m = MatrixComplex<T>(result.node());
      const ComplexNode<T> * d = static_cast<const ComplexNode<T> *>(nextOperandEvaluation.node());
      intermediateResult = computeOnMatrixAndComplex(m, *d);
    } else {
      assert(result.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      assert(nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      MatrixComplex<T> m = MatrixComplex<T>(result.node());
      MatrixComplex<T> n = MatrixComplex<T>(nextOperandEvaluation.node());
      intermediateResult = computeOnMatrices(m, n);
    }
    result = intermediateResult;
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

template<typename T> MatrixComplex<T> ApproximationEngine::elementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> m, const std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.isAllocationFailure()) {
    return MatrixComplex<T>(EvaluationNode<T>::FailedAllocationStaticNode());
  }
  MatrixComplex<T> matrix;
  for (int i = 0; i < m.numberOfChildren(); i++) {
    ComplexNode<T> * child = static_cast<MatrixComplexNode<T> *>(m.node())->childAtIndex(i);
    if (child) {
      matrix.addChildTreeAtIndex(computeOnComplexes(*child, c), i, i);
    } else {
      matrix.addChildTreeAtIndex(Complex<T>::Undefined(), i, i);
    }
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template<typename T> MatrixComplex<T> ApproximationEngine::elementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.isAllocationFailure() || n.isAllocationFailure()) {
    return MatrixComplex<T>(EvaluationNode<T>::FailedAllocationStaticNode());
  }
  if (m.numberOfRows() != n.numberOfRows() || m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> matrix;
  for (int i = 0; i < m.numberOfChildren(); i++) {
    ComplexNode<T> * childM = static_cast<MatrixComplexNode<T> *>(m.node())->childAtIndex(i);
    ComplexNode<T> * childN = static_cast<MatrixComplexNode<T> *>(n.node())->childAtIndex(i);
    if (childM && childN) {
      matrix.addChildTreeAtIndex(computeOnComplexes(*childM, *childN), i, i);
    } else {
      matrix.addChildTreeAtIndex(Complex<T>::Undefined(), i, i);
    }
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template std::complex<float> Poincare::ApproximationEngine::truncateRealOrImaginaryPartAccordingToArgument<float>(std::complex<float>);
template std::complex<double> Poincare::ApproximationEngine::truncateRealOrImaginaryPartAccordingToArgument<double>(std::complex<double>);
template Poincare::Evaluation<float> Poincare::ApproximationEngine::map(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<float> compute);
template Poincare::Evaluation<double> Poincare::ApproximationEngine::map(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<double> compute);
template Poincare::Evaluation<float> Poincare::ApproximationEngine::mapReduce(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<float> computeOnMatrices);
template Poincare::Evaluation<double> Poincare::ApproximationEngine::mapReduce(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<double> computeOnMatrices);
template Poincare::MatrixComplex<float> Poincare::ApproximationEngine::elementWiseOnMatrixComplexAndComplex<float>(const Poincare::MatrixComplex<float>, const std::complex<float>, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>));
template Poincare::MatrixComplex<double> Poincare::ApproximationEngine::elementWiseOnMatrixComplexAndComplex<double>(const Poincare::MatrixComplex<double>, std::complex<double> const, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>));
template Poincare::MatrixComplex<float> Poincare::ApproximationEngine::elementWiseOnComplexMatrices<float>(const Poincare::MatrixComplex<float>, const Poincare::MatrixComplex<float>, Poincare::Complex<float> (*)(std::complex<float>, std::complex<float>));
template Poincare::MatrixComplex<double> Poincare::ApproximationEngine::elementWiseOnComplexMatrices<double>(const Poincare::MatrixComplex<double>, const Poincare::MatrixComplex<double>, Poincare::Complex<double> (*)(std::complex<double>, std::complex<double>));


}
