extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/complex_matrix.h>
#include <poincare/complex.h>
#include "layout/grid_layout.h"
#include "layout/bracket_layout.h"
#include <math.h>
#include <float.h>
#include <string.h>

namespace Poincare {

ComplexMatrix::ComplexMatrix(const Complex * complexes, int numberOfRows, int numberOfColumns) :
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  assert(complexes != nullptr);
  m_values = new Complex[numberOfColumns*numberOfRows];
  for (int i = 0; i < numberOfColumns*numberOfRows; i++) {
    m_values[i] = complexes[i];
  }
}

ComplexMatrix::~ComplexMatrix() {
  delete[] m_values;
}

float ComplexMatrix::toFloat() const {
  if (m_numberOfColumns != 1 || m_numberOfRows != 1) {
    return NAN;
  }
  if (m_values[0].b() != 0.0f) {
    return NAN;
  }
  return m_values[0].a();
}

int ComplexMatrix::numberOfRows() const {
  return m_numberOfRows;
}

int ComplexMatrix::numberOfColumns() const {
  return m_numberOfColumns;
}

const Complex * ComplexMatrix::complexOperand(int i) const {
  return &m_values[i];
}

ComplexMatrix * ComplexMatrix::clone() const {
  return this->cloneWithDifferentOperands((Expression **)&m_values, m_numberOfRows*m_numberOfColumns);
}

ComplexMatrix * ComplexMatrix::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new ComplexMatrix((Complex *)newOperands[0], m_numberOfColumns, m_numberOfRows);
}

Evaluation * ComplexMatrix::createIdentity(int dim) {
  Complex * operands = new Complex [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = Complex(Complex::Float(1.0));
      } else {
        operands[i*dim+j] = Complex(Complex::Float(0.0));
      }
    }
  }
  Evaluation * matrix = new ComplexMatrix(operands, dim, dim);
  delete [] operands;
  return matrix;
}

}
