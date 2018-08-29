extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/evaluation.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
#include <poincare/expression.h>
#include <cmath>

namespace Poincare {

template<typename T>
T Complex<T>::toScalar() const {
  if (this->imag() == 0.0) {
    return this->real();
  }
  return NAN;
}

template<typename T>
Complex<T> * Complex<T>::createInverse() const {
  return new Complex<T>(Division::compute(std::complex<T>(1.0), *this));
}

template<typename T>
MatrixComplex<T>::MatrixComplex(std::complex<T> * operands, int numberOfRows, int numberOfColumns) :
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  m_operands = new std::complex<T> [numberOfRows*numberOfColumns];
  for (int i=0; i<numberOfRows*numberOfColumns; i++) {
    m_operands[i] = operands[i];
  }
}

template<typename T>
MatrixComplex<T>::~MatrixComplex() {
  if (m_operands != nullptr) {
    delete [] m_operands;
  }
}

template<typename T>
std::complex<T> MatrixComplex<T>::createTrace() const {
  if (numberOfRows() != numberOfColumns()) {
    return std::complex<T>(NAN, NAN);
  }
  int dim = numberOfRows();
  std::complex<T> c = std::complex<T>(0);
  for (int i = 0; i < dim; i++) {
    c += complexOperand(i*dim+i);
  }
  return c;
}

template<typename T>
std::complex<T> MatrixComplex<T>::createDeterminant() const {
  if (numberOfRows() != numberOfColumns()) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> * operandsCopy = new std::complex<T> [m_numberOfRows*m_numberOfColumns];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    operandsCopy[i] = m_operands[i];
  }
  std::complex<T> determinant = std::complex<T>(1);
  Matrix::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns, &determinant);
  delete[] operandsCopy;
  return determinant;
}

template<typename T>
MatrixComplex<T> * MatrixComplex<T>::createInverse() const {
  std::complex<T> * operandsCopy = new std::complex<T> [m_numberOfRows*m_numberOfColumns];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    operandsCopy[i] = m_operands[i];
  }
  int result = Matrix::ArrayInverse(operandsCopy, m_numberOfRows, m_numberOfColumns);
  MatrixComplex<T> * inverse = nullptr;
  if (result == 0) {
    // Intentionally swapping dimensions for inverse, although it doesn't make a difference because it is square
    inverse = new MatrixComplex<T>(operandsCopy, m_numberOfColumns, m_numberOfRows);
  }
  delete [] operandsCopy;
  return inverse;
}

template<typename T>
MatrixComplex<T> * MatrixComplex<T>::createTranspose() const {
  std::complex<T> * operands = new std::complex<T> [numberOfComplexOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = complexOperand(i*numberOfColumns()+j);
    }
  }
  // Intentionally swapping dimensions for transpose
  MatrixComplex<T> * matrix = new MatrixComplex<T>(operands, numberOfColumns(), numberOfRows());
  delete[] operands;
  return matrix;
}

template<typename T>
MatrixComplex<T> MatrixComplex<T>::createIdentity(int dim) {
  std::complex<T> * operands = new std::complex<T> [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = std::complex<T>(1);
      } else {
        operands[i*dim+j] = std::complex<T>(0);
      }
    }
  }
  MatrixComplex<T> matrix = MatrixComplex(operands, dim, dim);
  delete [] operands;
  return matrix;
}

template class Complex<float>;
template class Complex<double>;
template class MatrixComplex<float>;
template class MatrixComplex<double>;
}
