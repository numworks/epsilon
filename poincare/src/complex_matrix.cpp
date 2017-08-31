extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/complex_matrix.h>
#include <poincare/complex.h>
#include "layout/grid_layout.h"
#include "layout/bracket_layout.h"
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

template<typename T>
ComplexMatrix<T>::ComplexMatrix(const Complex<T> * complexes, int numberOfRows, int numberOfColumns) :
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  assert(complexes != nullptr);
  m_values = new Complex<T>[numberOfRows*numberOfColumns];
  for (int i = 0; i < numberOfRows*numberOfColumns; i++) {
    m_values[i] = complexes[i];
  }
}

template<typename T>
ComplexMatrix<T>::~ComplexMatrix() {
  delete[] m_values;
}

template<typename T>
T ComplexMatrix<T>::toScalar() const {
  if (m_numberOfRows != 1 || m_numberOfColumns != 1) {
    return NAN;
  }
  if (m_values[0].b() != 0) {
    return NAN;
  }
  return m_values[0].a();
}

template<typename T>
int ComplexMatrix<T>::numberOfRows() const {
  return m_numberOfRows;
}

template<typename T>
int ComplexMatrix<T>::numberOfColumns() const {
  return m_numberOfColumns;
}

template<typename T>
const Complex<T> * ComplexMatrix<T>::complexOperand(int i) const {
  return &m_values[i];
}

template<typename T>
ComplexMatrix<T> * ComplexMatrix<T>::clone() const {
  return new ComplexMatrix<T>(m_values, m_numberOfRows, m_numberOfColumns);
}

template<typename T>
ComplexMatrix<T> * ComplexMatrix<T>::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  return new ComplexMatrix((Complex<T> *)newOperands[0], m_numberOfRows, m_numberOfColumns);
}

template<typename T>
Evaluation<T> * ComplexMatrix<T>::createIdentity(int dim) {
  Complex<T> * operands = new Complex<T> [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = Complex<T>::Float(1.0);
      } else {
        operands[i*dim+j] = Complex<T>::Float(0.0);
      }
    }
  }
  Evaluation<T> * matrix = new ComplexMatrix<T>(operands, dim, dim);
  delete [] operands;
  return matrix;
}

template<typename T>
template <class U>
Evaluation<U> * ComplexMatrix<T>::templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const {
  Complex<U> * values = new Complex<U>[m_numberOfRows*m_numberOfColumns];
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    values[i] = Complex<U>::Cartesian(m_values[i].a(), m_values[i].b());
  }
  Evaluation<U> * result = new ComplexMatrix<U>(values, m_numberOfRows, m_numberOfColumns);
  delete [] values;
  return result;

}

template class Poincare::ComplexMatrix<float>;
template class Poincare::ComplexMatrix<double>;

}
