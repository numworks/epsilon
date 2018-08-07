extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/matrix_complex.h>
//#include <poincare/matrix.h>
#include <poincare/expression.h>
//#include <poincare/undefined.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

template<typename T>
ComplexNode<T> * childAtIndex(int index) {
  EvaluationNode<T> child = EvaluationNode<T>::childAtIndex(index);
  if (child->type() == EvaluationNode<T>::Type::Complex) {
    return static_cast<ComplexNode<T> *>(child);
  }
  return nullptr;
}

template<typename T>
bool MatrixComplexNode<T>::isUndefined() const {
  if (numberOfRows() != 1 || numberOfColumns() != 1) {
    return false;
  }
  ComplexNode<T> * child = childAtIndex(0);
  if (child && std::isnan((child->real()) && std::isnan(child->imag()))) {
    return true;
  }
  return false;
}

template<typename T>
Expression MatrixComplexNode<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  Matrix matrix;
  for (int i = 0; i < numberOfComplexOperands(); i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child) {
      matrix.addChildTreeAtIndex(child->complexToExpression(complexFormat), i, i);
    } else {
      matrix.addChildTreeAtIndex(Undefined(), i, i);
    }
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return matrix;
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::trace() const {
  if (numberOfRows() != numberOfColumns()) {
    return std::complex<T>(NAN, NAN);
  }
  int dim = numberOfRows();
  std::complex<T> c = std::complex<T>(0);
  for (int i = 0; i < dim; i++) {
    ComplexNode<T> * child = childAtIndex(i*dim+i);
    if (child == nullptr) {
      c = std::complex<T>(NAN, NAN);
      break;
    }
    c += *child;
  }
  return c;
}

template<typename T>
std::complex<T> MatrixComplexNode<T>::determinant() const {
  if (numberOfRows() != numberOfColumns() || numberOfComplexOperands() > Matrix::k_maxNumberOfCoefficients) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> operandsCopy[Matrix::k_maxNumberOfCoefficients];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child == nullptr) {
      return std::complex<T>(NAN, NAN);
    }
    operandsCopy[i] = *child;
  }
  std::complex<T> determinant = std::complex<T>(1);
  Matrix::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns, &determinant);
  return determinant;
}

template<typename T>
Evaluation<T> MatrixComplexNode<T>::inverse() const {
  if (numberOfRows() != numberOfColumns() || numberOfComplexOperands() > Matrix::k_maxNumberOfCoefficients) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[Matrix::k_maxNumberOfCoefficients];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child == nullptr) {
      return Evaluation<T>(EvaluationNode<T>::FailedAllocationStaticNode());
    }
    operandsCopy[i] = *child;
  }
  int result = Matrix::ArrayInverse(operandsCopy, m_numberOfRows, m_numberOfColumns);
  if (result == 0) {
    // Intentionally swapping dimensions for inverse, although it doesn't make a difference because it is square
    return MatrixComplex<T>(operandsCopy, m_numberOfColumns, m_numberOfRows);
  }
  return MatrixComplex<T>::Undefined();
}

template<typename T>
Evaluation<T> MatrixComplexNode<T>::transpose() const {
  // Intentionally swapping dimensions for transpose
  MatrixComplex<T> result;
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      ComplexNode<T> * child = childAtIndex(i*numberOfColumns()+i);
      if (child) {
        result.addChildTreeAtIndex(child, j*numberOfRows()+i, j*numberOfRows()+i);
      } else {
        result.addChildTreeAtIndex(Complex<T>::Undefined(), j*numberOfRows()+i, j*numberOfRows()+i);
      }
    }
  }
  result.setDimensions(numberOfColumns(), numberOfRows());
  return result;
}

// MATRIX COMPLEX REFERENCE

template<typename T>
MatrixComplex<T>::MatrixComplex(std::complex<T> * operands, int numberOfRows, int numberOfColumns) :
  MatrixComplex<T>()
{
  for (int i=0; i<numberOfRows*numberOfColumns; i++) {
    addChildTreeAtIndex(Complex<T>(operands[i]), i, i);
  }
  setDimensions(numberOfRows, numberOfColumns);
}

template<typename T>
MatrixComplex<T> MatrixComplex<T>::createIdentity(int dim) {
  MatrixComplex<T> result;
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      Complex<T> c = i == j ? Complex<T>(1.0) : Complex<T>(0.0);
      result.addChildTreeAtIndex(c, i*dim+j, i*dim+j);
    }
  }
  result.setDimensions(dim, dim);
  return result;
}

template<typename T>
int MatrixComplex<T>::numberOfRows() const {
  if (isAllocationFailure()) {
    return 0;
  }
  return typedNode()->numberOfRows();
}

template<typename T>
int MatrixComplex<T>::numberOfColumns() const {
  if (isAllocationFailure()) {
    return 0;
  }
  return typedNode()->numberOfColumns();
}

template<typename T>
void MatrixComplex<T>::setDimensions(int rows, int columns) {
  if (isAllocationFailure()) {
    return;
  }
  assert(rows * columns = numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

template<typename T>
void MatrixComplex<T>::addChildAtIndex(Complex<T> t, int index, int currentNumberOfChildren) {
  Expression::addChildAtIndex(t, index, currentNumberOfChildren);
  if (isAllocationFailure()) {
    return;
  }
  setNumberOfRows(1);
  setNumberOfColumns(currentNumberOfChildren + 1);
}

template<typename T>
void MatrixComplex<T>::setNumberOfRows(int rows) {
 if (isAllocationFailure()) {
    return;
  }
  assert(rows >= 0);
  typedNode()->setNumberOfRows(rows);
}

template<typename T>
void MatrixComplex<T>::setNumberOfColumns(int columns) {
  if (isAllocationFailure()) {
    return;
  }
  assert(columns >= 0);
  typedNode()->setNumberOfColumns(columns);
}

template class MatrixComplex<float>;
template class MatrixComplex<double>;

}
