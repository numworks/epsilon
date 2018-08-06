extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/matrix_complex.h>
#include <poincare/matrix.h>
#include <poincare/expression_reference.h>
#include <poincare/undefined.h>
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
ExpressionReference MatrixComplexNode<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  MatrixReference matrix;
  for (int i = 0; i < numberOfComplexOperands(); i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child) {
      matrix.addChildTreeAtIndex(child->complexToExpression(complexFormat), i, i);
    } else {
      matrix.addChildTreeAtIndex(UndefinedReference(), i, i);
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
  if (numberOfRows() != numberOfColumns() || numberOfComplexOperands() > MatrixReference::k_maxNumberOfCoefficients) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> operandsCopy[MatrixReference::k_maxNumberOfCoefficients];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child == nullptr) {
      return std::complex<T>(NAN, NAN);
    }
    operandsCopy[i] = *child;
  }
  std::complex<T> determinant = std::complex<T>(1);
  MatrixReference::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns, &determinant);
  return determinant;
}

template<typename T>
EvaluationReference<T> MatrixComplexNode<T>::inverse() const {
  if (numberOfRows() != numberOfColumns() || numberOfComplexOperands() > MatrixReference::k_maxNumberOfCoefficients) {
    return MatrixComplexReference<T>::Undefined();
  }
  std::complex<T> operandsCopy[MatrixReference::k_maxNumberOfCoefficients];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child == nullptr) {
      return EvaluationReference<T>(EvaluationNode<T>::FailedAllocationStaticNode());
    }
    operandsCopy[i] = *child;
  }
  int result = MatrixReference::ArrayInverse(operandsCopy, m_numberOfRows, m_numberOfColumns);
  if (result == 0) {
    // Intentionally swapping dimensions for inverse, although it doesn't make a difference because it is square
    return MatrixComplexReference<T>(operandsCopy, m_numberOfColumns, m_numberOfRows);
  }
  return MatrixComplexReference<T>::Undefined();
}

template<typename T>
EvaluationReference<T> MatrixComplexNode<T>::transpose() const {
  // Intentionally swapping dimensions for transpose
  MatrixComplexReference<T> result;
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      ComplexNode<T> * child = childAtIndex(i*numberOfColumns()+i);
      if (child) {
        result.addChildTreeAtIndex(child, j*numberOfRows()+i, j*numberOfRows()+i);
      } else {
        result.addChildTreeAtIndex(ComplexReference<T>::Undefined(), j*numberOfRows()+i, j*numberOfRows()+i);
      }
    }
  }
  result.setDimensions(numberOfColumns(), numberOfRows());
  return result;
}

// MATRIX COMPLEX REFERENCE

template<typename T>
MatrixComplexReference<T>::MatrixComplexReference(std::complex<T> * operands, int numberOfRows, int numberOfColumns) :
  MatrixComplexReference<T>()
{
  for (int i=0; i<numberOfRows*numberOfColumns; i++) {
    addChildTreeAtIndex(ComplexReference<T>(operands[i]), i, i);
  }
  setDimensions(numberOfRows, numberOfColumns);
}

template<typename T>
MatrixComplexReference<T> MatrixComplexReference<T>::createIdentity(int dim) {
  MatrixComplexReference<T> result;
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      ComplexReference<T> c = i == j ? ComplexReference<T>(1.0) : ComplexReference<T>(0.0);
      result.addChildTreeAtIndex(c, i*dim+j, i*dim+j);
    }
  }
  result.setDimensions(dim, dim);
  return result;
}

template<typename T>
int MatrixComplexReference<T>::numberOfRows() const {
  if (isAllocationFailure()) {
    return 0;
  }
  return typedNode()->numberOfRows();
}

template<typename T>
int MatrixComplexReference<T>::numberOfColumns() const {
  if (isAllocationFailure()) {
    return 0;
  }
  return typedNode()->numberOfColumns();
}

template<typename T>
void MatrixComplexReference<T>::setDimensions(int rows, int columns) {
  if (isAllocationFailure()) {
    return;
  }
  assert(rows * columns = numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

template<typename T>
void MatrixComplexReference<T>::addChildTreeAtIndex(TreeReference t, int index, int currentNumberOfChildren) {
  ExpressionReference::addChildTreeAtIndex(t, index, currentNumberOfChildren);
  if (isAllocationFailure()) {
    return;
  }
  setNumberOfRows(1);
  setNumberOfColumns(currentNumberOfChildren + 1);
}

template<typename T>
void MatrixComplexReference<T>::setNumberOfRows(int rows) {
 if (isAllocationFailure()) {
    return;
  }
  assert(rows >= 0);
  typedNode()->setNumberOfRows(rows);
}

template<typename T>
void MatrixComplexReference<T>::setNumberOfColumns(int columns) {
  if (isAllocationFailure()) {
    return;
  }
  assert(columns >= 0);
  typedNode()->setNumberOfColumns(columns);
}

template class MatrixComplexReference<float>;
template class MatrixComplexReference<double>;

}
