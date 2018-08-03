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

template<typename T> EvaluationReference<T> ApproximationEngine::map(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute) {
  assert(expression->numberOfChildren() == 1);
  EvaluationReference<T> input = expression->childAtIndex(0)->approximate(T(), context, angleUnit);
  if (input.node()->type() == EvaluationNode<T>::Type::AllocationFailure) {
    return EvaluationReference<T>(EvaluationNode<T>::FailedAllocationStaticNode());
  } else if (input.node()->type() == EvaluationNode<T>::Type::Complex) {
    const ComplexNode<T> * c = static_cast<ComplexNode<T> *>(input.node());
    return compute(*c, angleUnit);
  } else {
    assert(input.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
    MatrixComplexReference<T> m = MatrixComplexReference<T>(input.node());
    MatrixComplexReference<T> result;
    for (int i = 0; i < result.numberOfRows()*result.numberOfColumns(); i++) {
      ComplexNode<T> * child = static_cast<MatrixComplexNode<T> *>(m.node())->childAtIndex(i);
      if (child) {
        result.addChildTreeAtIndex(compute(*child, angleUnit), i, i);
      } else {
        result.addChildTreeAtIndex(ComplexReference<T>::Undefined(), i, i);
      }
    }
    result.setDimensions(m.numberOfRows(), m.numberOfColumns());
    return result;
  }
}

template<typename T> EvaluationReference<T> ApproximationEngine::mapReduce(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices) {
  EvaluationReference<T> result = expression->childAtIndex(0)->approximate(T(), context, angleUnit);
  for (int i = 1; i < expression->numberOfChildren(); i++) {
    EvaluationReference<T> intermediateResult(nullptr);
    EvaluationReference<T> nextOperandEvaluation = expression->childAtIndex(i)->approximate(T(), context, angleUnit);
    if (result.node()->type() == EvaluationNode<T>::Type::AllocationFailure || nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::AllocationFailure) {
      return EvaluationReference<T>(EvaluationNode<T>::FailedAllocationStaticNode());
    }
    if (result.node()->type() == EvaluationNode<T>::Type::Complex && nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::Complex) {
      const ComplexNode<T> * c = static_cast<const ComplexNode<T> *>(result.node());
      const ComplexNode<T> * d = static_cast<const ComplexNode<T> *>(nextOperandEvaluation.node());
      intermediateResult = computeOnComplexes(*c, *d);
    } else if (result.node()->type() == EvaluationNode<T>::Type::Complex) {
      assert(nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      const ComplexNode<T> * c = static_cast<const ComplexNode<T> *>(result.node());
      MatrixComplexReference<T> n = MatrixComplexReference<T>(nextOperandEvaluation.node());
      intermediateResult = computeOnComplexAndMatrix(*c, n);
    } else if (nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::Complex) {
      assert(result.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      MatrixComplexReference<T> m = MatrixComplexReference<T>(result.node());
      const ComplexNode<T> * d = static_cast<const ComplexNode<T> *>(nextOperandEvaluation.node());
      intermediateResult = computeOnMatrixAndComplex(m, *d);
    } else {
      assert(result.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      assert(nextOperandEvaluation.node()->type() == EvaluationNode<T>::Type::MatrixComplex);
      MatrixComplexReference<T> m = MatrixComplexReference<T>(result.node());
      MatrixComplexReference<T> n = MatrixComplexReference<T>(nextOperandEvaluation.node());
      intermediateResult = computeOnMatrices(m, n);
    }
    result = intermediateResult;
    if (result.isUndefined()) {
      return ComplexReference<T>::Undefined();
    }
  }
  return result;
}

template<typename T> MatrixComplexReference<T> ApproximationEngine::elementWiseOnMatrixComplexAndComplex(const MatrixComplexReference<T> m, const std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.isAllocationFailure()) {
    return MatrixComplexReference<T>(EvaluationNode<T>::FailedAllocationStaticNode());
  }
  MatrixComplexReference<T> matrix;
  for (int i = 0; i < m.numberOfChildren(); i++) {
    ComplexNode<T> * child = static_cast<MatrixComplexNode<T> *>(m.node())->childAtIndex(i);
    if (child) {
      matrix.addChildTreeAtIndex(computeOnComplexes(*child, c), i, i);
    } else {
      matrix.addChildTreeAtIndex(ComplexReference<T>::Undefined(), i, i);
    }
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template<typename T> MatrixComplexReference<T> ApproximationEngine::elementWiseOnComplexMatrices(const MatrixComplexReference<T> m, const MatrixComplexReference<T> n, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.isAllocationFailure() || n.isAllocationFailure()) {
    return MatrixComplexReference<T>(EvaluationNode<T>::FailedAllocationStaticNode());
  }
  if (m.numberOfRows() != n.numberOfRows() || m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplexReference<T>::Undefined();
  }
  MatrixComplexReference<T> matrix;
  for (int i = 0; i < m.numberOfChildren(); i++) {
    ComplexNode<T> * childM = static_cast<MatrixComplexNode<T> *>(m.node())->childAtIndex(i);
    ComplexNode<T> * childN = static_cast<MatrixComplexNode<T> *>(n.node())->childAtIndex(i);
    if (childM && childN) {
      matrix.addChildTreeAtIndex(computeOnComplexes(*childM, *childN), i, i);
    } else {
      matrix.addChildTreeAtIndex(ComplexReference<T>::Undefined(), i, i);
    }
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

template std::complex<float> Poincare::ApproximationEngine::truncateRealOrImaginaryPartAccordingToArgument<float>(std::complex<float>);
template std::complex<double> Poincare::ApproximationEngine::truncateRealOrImaginaryPartAccordingToArgument<double>(std::complex<double>);
template Poincare::EvaluationReference<float> Poincare::ApproximationEngine::map(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<float> compute);
template Poincare::EvaluationReference<double> Poincare::ApproximationEngine::map(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<double> compute);
template Poincare::EvaluationReference<float> Poincare::ApproximationEngine::mapReduce(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<float> computeOnMatrices);
template Poincare::EvaluationReference<double> Poincare::ApproximationEngine::mapReduce(const Poincare::ExpressionNode * expression, Poincare::Context& context, Poincare::Preferences::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<double> computeOnMatrices);
template Poincare::MatrixComplexReference<float> Poincare::ApproximationEngine::elementWiseOnMatrixComplexAndComplex<float>(const Poincare::MatrixComplexReference<float>, const std::complex<float>, Poincare::ComplexReference<float> (*)(std::complex<float>, std::complex<float>));
template Poincare::MatrixComplexReference<double> Poincare::ApproximationEngine::elementWiseOnMatrixComplexAndComplex<double>(const Poincare::MatrixComplexReference<double>, std::complex<double> const, Poincare::ComplexReference<double> (*)(std::complex<double>, std::complex<double>));
template Poincare::MatrixComplexReference<float> Poincare::ApproximationEngine::elementWiseOnComplexMatrices<float>(const Poincare::MatrixComplexReference<float>, const Poincare::MatrixComplexReference<float>, Poincare::ComplexReference<float> (*)(std::complex<float>, std::complex<float>));
template Poincare::MatrixComplexReference<double> Poincare::ApproximationEngine::elementWiseOnComplexMatrices<double>(const Poincare::MatrixComplexReference<double>, const Poincare::MatrixComplexReference<double>, Poincare::ComplexReference<double> (*)(std::complex<double>, std::complex<double>));


}
