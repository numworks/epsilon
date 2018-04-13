#include <poincare/approximation_engine.h>
#include <poincare/evaluation.h>
#include <poincare/matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<typename T> Evaluation<T> * ApproximationEngine::map(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexCompute<T> compute) {
  assert(expression->numberOfOperands() == 1);
  Evaluation<T> * input = expression->operand(0)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * result = nullptr;
  if (input->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(input);
    result = new Complex<T>(compute(*c, angleUnit));
  } else {
    assert(input->type() == Evaluation<T>::Type::MatrixComplex);
    MatrixComplex<T> * m = static_cast<MatrixComplex<T> *>(input);
    std::complex<T> * operands = new std::complex<T> [m->numberOfComplexOperands()];
    for (int i = 0; i < m->numberOfComplexOperands(); i++) {
      const std::complex<T> c = m->complexOperand(i);
      operands[i] = compute(c, angleUnit);
    }
    result = new MatrixComplex<T>(operands, m->numberOfRows(), m->numberOfColumns());
    delete[] operands;
  }
  delete input;
  return result;
}

template<typename T> Evaluation<T> * ApproximationEngine::mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices) {
  Evaluation<T> * result = expression->operand(0)->privateApproximate(T(), context, angleUnit);
  for (int i = 1; i < expression->numberOfOperands(); i++) {
    Evaluation<T> * intermediateResult = nullptr;
    Evaluation<T> * nextOperandEvaluation = expression->operand(i)->privateApproximate(T(), context, angleUnit);
    if (result->type() == Evaluation<T>::Type::Complex && nextOperandEvaluation->type() == Evaluation<T>::Type::Complex) {
      const Complex<T> * c = static_cast<const Complex<T> *>(result);
      const Complex<T> * d = static_cast<const Complex<T> *>(nextOperandEvaluation);
      intermediateResult = new Complex<T>(computeOnComplexes(*c, *d));
    } else if (result->type() == Evaluation<T>::Type::Complex) {
      const Complex<T> * c = static_cast<const Complex<T> *>(result);
      assert(nextOperandEvaluation->type() == Evaluation<T>::Type::MatrixComplex);
      const MatrixComplex<T> * n = static_cast<const MatrixComplex<T> *>(nextOperandEvaluation);
      intermediateResult = new MatrixComplex<T>(computeOnComplexAndMatrix(*c, *n));
    } else if (nextOperandEvaluation->type() == Evaluation<T>::Type::Complex) {
      assert(result->type() == Evaluation<T>::Type::MatrixComplex);
      const MatrixComplex<T> * m = static_cast<const MatrixComplex<T> *>(result);
      const Complex<T> * d = static_cast<const Complex<T> *>(nextOperandEvaluation);
      intermediateResult = new MatrixComplex<T>(computeOnMatrixAndComplex(*m, *d));
    } else {
      assert(result->type() == Evaluation<T>::Type::MatrixComplex);
      const MatrixComplex<T> * m = static_cast<const MatrixComplex<T> *>(result);
      assert(nextOperandEvaluation->type() == Evaluation<T>::Type::MatrixComplex);
      const MatrixComplex<T> * n = static_cast<const MatrixComplex<T> *>(nextOperandEvaluation);
      intermediateResult = new MatrixComplex<T>(computeOnMatrices(*m, *n));
    }
    delete result;
    delete nextOperandEvaluation;
    result = intermediateResult;
    if (result == nullptr) {
      return new Complex<T>(Complex<T>::Undefined());
    }
  }
  return result;
}

template<typename T> MatrixComplex<T> ApproximationEngine::elementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> m, const std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes) {
  std::complex<T> * operands = new std::complex<T> [m.numberOfRows()*m.numberOfColumns()];
  for (int i = 0; i < m.numberOfComplexOperands(); i++) {
    const std::complex<T> d = m.complexOperand(i);
    operands[i] = computeOnComplexes(d, c);
  }
  MatrixComplex<T> result = MatrixComplex<T>(operands, m.numberOfRows(), m.numberOfColumns());
  delete[] operands;
  return result;
}

template<typename T> MatrixComplex<T> ApproximationEngine::elementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m.numberOfRows() != n.numberOfRows() || m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> * operands = new std::complex<T> [m.numberOfRows()*m.numberOfColumns()];
  for (int i = 0; i < m.numberOfComplexOperands(); i++) {
    const Complex<T> c = m.complexOperand(i);
    const Complex<T> d = n.complexOperand(i);
    operands[i] = computeOnComplexes(c, d);
  }
  MatrixComplex<T> result = MatrixComplex<T>(operands, m.numberOfRows(), m.numberOfColumns());
  delete[] operands;
  return result;
}

template Poincare::Evaluation<float> * Poincare::ApproximationEngine::map(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<float> compute);
template Poincare::Evaluation<double> * Poincare::ApproximationEngine::map(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<double> compute);
template Poincare::Evaluation<float> * Poincare::ApproximationEngine::mapReduce(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<float> computeOnMatrices);
template Poincare::Evaluation<double> * Poincare::ApproximationEngine::mapReduce(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<double> computeOnMatrices);
template Poincare::MatrixComplex<float> Poincare::ApproximationEngine::elementWiseOnMatrixComplexAndComplex<float>(const Poincare::MatrixComplex<float>, const std::complex<float>, std::complex<float> (*)(std::complex<float>, std::complex<float>));
template Poincare::MatrixComplex<double> Poincare::ApproximationEngine::elementWiseOnMatrixComplexAndComplex<double>(const Poincare::MatrixComplex<double>, std::complex<double> const, std::complex<double> (*)(std::complex<double>, std::complex<double>));
template Poincare::MatrixComplex<float> Poincare::ApproximationEngine::elementWiseOnComplexMatrices<float>(const Poincare::MatrixComplex<float>, const Poincare::MatrixComplex<float>, std::complex<float> (*)(std::complex<float>, std::complex<float>));
template Poincare::MatrixComplex<double> Poincare::ApproximationEngine::elementWiseOnComplexMatrices<double>(const Poincare::MatrixComplex<double>, const Poincare::MatrixComplex<double>, std::complex<double> (*)(std::complex<double>, std::complex<double>));


}
