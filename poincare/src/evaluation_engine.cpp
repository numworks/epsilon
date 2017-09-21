#include <poincare/evaluation_engine.h>
#include <poincare/complex_matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

namespace Poincare {

template<typename T> Evaluation<T> * EvaluationEngine::map(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ExpressionToComplexMap<T> compute) {
  assert(expression->numberOfOperands() == 1);
  Evaluation<T> * input = expression->operand(0)->evaluate<T>(context, angleUnit);
  Complex<T> * operands = new Complex<T>[input->numberOfRows()*input->numberOfColumns()];
  for (int i = 0; i < input->numberOfOperands(); i++) {
    operands[i] = compute(*input->complexOperand(i), angleUnit);
  }
  Evaluation<T> * result = nullptr;
  if (input->numberOfOperands() == 1) {
    result = new Complex<T>(operands[0]);
  } else {
    result = new ComplexMatrix<T>(operands, input->numberOfRows(), input->numberOfColumns());
  }
  delete input;
  delete[] operands;
  return result;
}

template<typename T> Evaluation<T> * EvaluationEngine::mapReduce(const Expression * expression, Context& context, Expression::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices) {
  Evaluation<T> * result = expression->operand(0)->evaluate<T>(context, angleUnit);
  for (int i = 1; i < expression->numberOfOperands(); i++) {
    Evaluation<T> * intermediateResult = nullptr;
    Evaluation<T> * nextOperandEvaluation = expression->operand(i)->evaluate<T>(context, angleUnit);
    if (result->numberOfRows() == 1 && result->numberOfColumns() == 1 && nextOperandEvaluation->numberOfRows() == 1 && nextOperandEvaluation->numberOfColumns() == 1) {
      intermediateResult = new Complex<T>(computeOnComplexes(*(result->complexOperand(0)), *(nextOperandEvaluation->complexOperand(0))));
    } else if (result->numberOfRows() == 1 && result->numberOfColumns() == 1) {
      intermediateResult = computeOnComplexAndMatrix(result->complexOperand(0), nextOperandEvaluation);
    } else if (nextOperandEvaluation->numberOfRows() == 1 && nextOperandEvaluation->numberOfColumns() == 1) {
      intermediateResult = computeOnMatrixAndComplex(result, nextOperandEvaluation->complexOperand(0));
    } else {
      intermediateResult = computeOnMatrices(result, nextOperandEvaluation);
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

template<typename T> Evaluation<T> * EvaluationEngine::elementWiseOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * m, ComplexAndComplexReduction<T> computeOnComplexes) {
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = computeOnComplexes(*(m->complexOperand(i)), *c);
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

template<typename T> Evaluation<T> * EvaluationEngine::elementWiseOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n, ComplexAndComplexReduction<T> computeOnComplexes) {
  if (m->numberOfRows() != n->numberOfRows() && m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = computeOnComplexes(*(m->complexOperand(i)), *(n->complexOperand(i)));
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

template Poincare::Evaluation<float> * Poincare::EvaluationEngine::map(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::EvaluationEngine::ExpressionToComplexMap<float> compute);
template Poincare::Evaluation<double> * Poincare::EvaluationEngine::map(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::EvaluationEngine::ExpressionToComplexMap<double> compute);
template Poincare::Evaluation<float> * Poincare::EvaluationEngine::mapReduce(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::EvaluationEngine::ComplexAndComplexReduction<float> computeOnComplexes, Poincare::EvaluationEngine::ComplexAndMatrixReduction<float> computeOnComplexAndMatrix, Poincare::EvaluationEngine::MatrixAndComplexReduction<float> computeOnMatrixAndComplex, Poincare::EvaluationEngine::MatrixAndMatrixReduction<float> computeOnMatrices);
template Poincare::Evaluation<double> * Poincare::EvaluationEngine::mapReduce(const Poincare::Expression * expression, Poincare::Context& context, Poincare::Expression::AngleUnit angleUnit, Poincare::EvaluationEngine::ComplexAndComplexReduction<double> computeOnComplexes, Poincare::EvaluationEngine::ComplexAndMatrixReduction<double> computeOnComplexAndMatrix, Poincare::EvaluationEngine::MatrixAndComplexReduction<double> computeOnMatrixAndComplex, Poincare::EvaluationEngine::MatrixAndMatrixReduction<double> computeOnMatrices);
template Poincare::Evaluation<float>* Poincare::EvaluationEngine::elementWiseOnComplexAndComplexMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*, Poincare::Complex<float> (*)(Poincare::Complex<float>, Poincare::Complex<float>));
template Poincare::Evaluation<double>* Poincare::EvaluationEngine::elementWiseOnComplexAndComplexMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*, Poincare::Complex<double> (*)(Poincare::Complex<double>, Poincare::Complex<double>));
template Poincare::Evaluation<float>* Poincare::EvaluationEngine::elementWiseOnComplexMatrices<float>(Poincare::Evaluation<float>*, Poincare::Evaluation<float>*, Poincare::Complex<float> (*)(Poincare::Complex<float>, Poincare::Complex<float>));
template Poincare::Evaluation<double>* Poincare::EvaluationEngine::elementWiseOnComplexMatrices<double>(Poincare::Evaluation<double>*, Poincare::Evaluation<double>*, Poincare::Complex<double> (*)(Poincare::Complex<double>, Poincare::Complex<double>));

}
