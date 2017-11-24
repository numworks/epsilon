#include <poincare/approximation_engine.h>
#include <poincare/matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<typename T> Expression * ApproximationEngine::map(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexCompute<T> compute) {
  assert(expression->numberOfOperands() == 1);
  Expression * input = expression->operand(0)->approximate<T>(context, angleUnit);
  Expression * result = nullptr;
  if (input->type() == Expression::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(input);
    result = new Complex<T>(compute(*c, angleUnit));
  } else {
    assert(input->type() == Expression::Type::Matrix);
    Expression ** operands = new Expression * [input->numberOfOperands()];
    for (int i = 0; i < input->numberOfOperands(); i++) {
      assert(input->operand(i)->type() == Expression::Type::Complex);
      const Complex<T> * c = static_cast<const Complex<T> *>(input->operand(i));
      operands[i] = new Complex<T>(compute(*c, angleUnit));
    }
    result = new Matrix(operands, static_cast<Matrix *>(input)->numberOfRows(), static_cast<Matrix *>(input)->numberOfColumns(), false);
    delete[] operands;
  }
  delete input;
  return result;
}

template<typename T> Expression * ApproximationEngine::mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices) {
  Expression * result = expression->operand(0)->approximate<T>(context, angleUnit);
  for (int i = 1; i < expression->numberOfOperands(); i++) {
    Expression * intermediateResult = nullptr;
    Expression * nextOperandEvaluation = expression->operand(i)->approximate<T>(context, angleUnit);
    if (result->type() == Expression::Type::Complex && nextOperandEvaluation->type() == Expression::Type::Complex) {
      const Complex<T> * c = static_cast<const Complex<T> *>(result);
      const Complex<T> * d = static_cast<const Complex<T> *>(nextOperandEvaluation);
      intermediateResult = new Complex<T>(computeOnComplexes(*c, *d));
    } else if (result->type() == Expression::Type::Complex) {
      const Complex<T> * c = static_cast<const Complex<T> *>(result);
      assert(nextOperandEvaluation->type() == Expression::Type::Matrix);
      const Matrix * n = static_cast<const Matrix *>(nextOperandEvaluation);
      intermediateResult = computeOnComplexAndMatrix(c, n);
    } else if (nextOperandEvaluation->type() == Expression::Type::Complex) {
      assert(result->type() == Expression::Type::Matrix);
      const Matrix * m = static_cast<const Matrix *>(result);
      const Complex<T> * d = static_cast<const Complex<T> *>(nextOperandEvaluation);
      intermediateResult = computeOnMatrixAndComplex(m, d);
    } else {
      assert(result->type() == Expression::Type::Matrix);
      const Matrix * m = static_cast<const Matrix *>(result);
      assert(nextOperandEvaluation->type() == Expression::Type::Matrix);
      const Matrix * n = static_cast<const Matrix *>(nextOperandEvaluation);
      intermediateResult = computeOnMatrices(m, n);
    }
    delete result;
    delete nextOperandEvaluation;
    result = intermediateResult;
    if (result == nullptr) {
      return new Complex<T>(Complex<T>::Float(NAN));
    }
  }
  return result;
}

template<typename T> Matrix * ApproximationEngine::elementWiseOnComplexAndComplexMatrix(const Complex<T> * c, const Matrix * m, ComplexAndComplexReduction<T> computeOnComplexes) {
  Expression ** operands = new Expression * [m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    const Complex<T> * d = static_cast<const Complex<T> *>(m->operand(i));
    operands[i] = new Complex<T>(computeOnComplexes(*d, *c));
  }
  Matrix * result = new Matrix(operands, m->numberOfRows(), m->numberOfColumns(), false);
  delete[] operands;
  return result;
}

template<typename T> Matrix * ApproximationEngine::elementWiseOnComplexMatrices(const Matrix * m, const Matrix * n, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m->numberOfRows() != n->numberOfRows() || m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Expression ** operands = new Expression * [m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    const Complex<T> * c = static_cast<const Complex<T> *>(m->operand(i));
    const Complex<T> * d = static_cast<const Complex<T> *>(n->operand(i));
    operands[i] = new Complex<T>(computeOnComplexes(*c, *d));
  }
  Matrix * result = new Matrix(operands, m->numberOfRows(), m->numberOfColumns(), false);
  delete[] operands;
  return result;
}

template Poincare::Expression * Poincare::ApproximationEngine::map(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<float> compute);
template Poincare::Expression * Poincare::ApproximationEngine::map(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexCompute<double> compute);
template Poincare::Expression * Poincare::ApproximationEngine::mapReduce(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<float> computeOnMatrices);
template Poincare::Expression * Poincare::ApproximationEngine::mapReduce(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::ApproximationEngine::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::ApproximationEngine::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::ApproximationEngine::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::ApproximationEngine::MatrixAndMatrixReduction<double> computeOnMatrices);
template Poincare::Matrix * Poincare::ApproximationEngine::elementWiseOnComplexAndComplexMatrix<float>(Poincare::Complex<float> const*, const Poincare::Matrix *, Poincare::Complex<float> (*)(Poincare::Complex<float>, Poincare::Complex<float>));
template Poincare::Matrix* Poincare::ApproximationEngine::elementWiseOnComplexAndComplexMatrix<double>(Poincare::Complex<double> const*, const Poincare::Matrix*, Poincare::Complex<double> (*)(Poincare::Complex<double>, Poincare::Complex<double>));
template Poincare::Matrix* Poincare::ApproximationEngine::elementWiseOnComplexMatrices<float>(const Poincare::Matrix*, const Poincare::Matrix*, Poincare::Complex<float> (*)(Poincare::Complex<float>, Poincare::Complex<float>));
template Poincare::Matrix* Poincare::ApproximationEngine::elementWiseOnComplexMatrices<double>(const Poincare::Matrix*, const Poincare::Matrix*, Poincare::Complex<double> (*)(Poincare::Complex<double>, Poincare::Complex<double>));


}
