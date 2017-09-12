#include <poincare/n_ary_operation.h>
#include <poincare/complex_matrix.h>
#include <cmath>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"
#include "layout/horizontal_layout.h"

namespace Poincare {

NAryOperation::NAryOperation() :
  m_operands(nullptr),
  m_numberOfOperands(0)
{
}

NAryOperation::NAryOperation(Expression ** operands, int numberOfOperands, bool cloneOperands) :
  m_numberOfOperands(numberOfOperands)
{
  assert(operands != nullptr);
  assert(numberOfOperands >= 2);
  m_operands = new Expression * [numberOfOperands];
  for (int i=0; i<numberOfOperands; i++) {
    assert(operands[i] != nullptr);
    if (cloneOperands) {
      m_operands[i] = operands[i]->clone();
    } else {
      m_operands[i] = operands[i];
    }
  }
}

NAryOperation::~NAryOperation() {
  if (m_operands != nullptr) {
    for (int i = 0; i < m_numberOfOperands; i++) {
      delete m_operands[i];
    }
  }
  delete[] m_operands;
}

bool NAryOperation::hasValidNumberOfArguments() const {
  for (int i = 0; i < m_numberOfOperands; i++) {
    if (!m_operands[i]->hasValidNumberOfArguments()) {
      return false;
    }
  }
  return true;
}

int NAryOperation::numberOfOperands() const {
  return m_numberOfOperands;
}

const Expression * NAryOperation::operand(int i) const {
  assert(i >= 0);
  assert(i < m_numberOfOperands);
  return m_operands[i];
}

Expression * NAryOperation::clone() const {
  return this->cloneWithDifferentOperands((Expression**) m_operands, m_numberOfOperands, true);
}

ExpressionLayout * NAryOperation::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout** children_layouts = new ExpressionLayout * [2*m_numberOfOperands-1];
  char string[2] = {operatorChar(), '\0'};
  children_layouts[0] = m_operands[0]->createLayout();
  for (int i=1; i<m_numberOfOperands; i++) {
    children_layouts[2*i-1] = new StringLayout(string, 1);
    children_layouts[2*i] = m_operands[i]->type() == Type::Opposite ? new ParenthesisLayout(m_operands[i]->createLayout(floatDisplayMode, complexFormat)) : m_operands[i]->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * layout = new HorizontalLayout(children_layouts, 2*m_numberOfOperands-1);
  delete[] children_layouts;
  return layout;
}

template<typename T> Evaluation<T> * NAryOperation::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * result = m_operands[0]->evaluate<T>(context, angleUnit);
  for (int i = 1; i < m_numberOfOperands; i++) {
    Evaluation<T> * intermediateResult = nullptr;
    Evaluation<T> * nextOperandEvaluation = m_operands[i]->evaluate<T>(context, angleUnit);
    if (result->numberOfRows() == 1 && result->numberOfColumns() == 1 && nextOperandEvaluation->numberOfRows() == 1 && nextOperandEvaluation->numberOfColumns() == 1) {
      intermediateResult = new Complex<T>(privateCompute(*(result->complexOperand(0)), *(nextOperandEvaluation->complexOperand(0))));
    } else if (result->numberOfRows() == 1 && result->numberOfColumns() == 1) {
      intermediateResult = computeOnComplexAndComplexMatrix(result->complexOperand(0), nextOperandEvaluation);
    } else if (nextOperandEvaluation->numberOfRows() == 1 && nextOperandEvaluation->numberOfColumns() == 1) {
      intermediateResult = computeOnComplexMatrixAndComplex(result, nextOperandEvaluation->complexOperand(0));
    } else {
      intermediateResult = computeOnComplexMatrices(result, nextOperandEvaluation);
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

template<typename T> Evaluation<T> * NAryOperation::templatedComputeOnComplexAndComplexMatrix(const Complex<T> * c, Evaluation<T> * n) const {
  return computeOnComplexMatrixAndComplex(n, c);
}

template<typename T> Evaluation<T> * NAryOperation::templatedComputeOnComplexMatrixAndComplex(Evaluation<T> * m, const Complex<T> * d) const {
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = privateCompute(*(m->complexOperand(i)), *d);
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

template<typename T> Evaluation<T> * NAryOperation::templatedComputeOnComplexMatrices(Evaluation<T> * m, Evaluation<T> * n) const {
  if (m->numberOfRows() != n->numberOfRows() && m->numberOfColumns() != n->numberOfColumns()) {
    return nullptr;
  }
  Complex<T> * operands = new Complex<T>[m->numberOfRows()*m->numberOfColumns()];
  for (int i = 0; i < m->numberOfOperands(); i++) {
    operands[i] = privateCompute(*(m->complexOperand(i)), *(n->complexOperand(i)));
  }
  Evaluation<T> * result = new ComplexMatrix<T>(operands, m->numberOfRows(), m->numberOfColumns());
  delete[] operands;
  return result;
}

char NAryOperation::operatorChar() const {
  return '.';
}

}

template Poincare::Evaluation<float>* Poincare::NAryOperation::templatedComputeOnComplexAndComplexMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*) const;
template Poincare::Evaluation<double>* Poincare::NAryOperation::templatedComputeOnComplexAndComplexMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*) const;
