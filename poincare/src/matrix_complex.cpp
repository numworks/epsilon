extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/matrix_complex.h>
//#include <poincare/matrix.h>
#include <poincare/expression_reference.h>
#include <poincare/undefined.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

template<typename T>
void MatrixComplexNode<T>::setMatrixComplexDimension(int numberOfRows, int numberOfColumns) {
  m_numberOfRows = numberOfRows;
  m_numberOfColumns = numberOfColumns;
}

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
  MatrixReference matrix(numberOfRows(), numberOfColumns());
  for (int i = 0; i < numberOfComplexOperands(); i++) {
    ComplexNode<T> * child = childAtIndex(i);
    if (child) {
      matrix.addChildTreeAtIndex(child->complexToExpression(complexFormat), i, i);
    } else {
      matrix.addChildTreeAtIndex(UndefinedReference(), i, i);
    }
  }
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
  if (numberOfRows() != numberOfColumns() || numberOfComplexOperands() > k_maxNumberOfCoefficients) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> operandsCopy[k_maxNumberOfCoefficients];
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
  if (numberOfRows() != numberOfColumns() || numberOfComplexOperands() > k_maxNumberOfCoefficients) {
    return MatrixComplexReference<T>::Undefined();
  }
  std::complex<T> operandsCopy[k_maxNumberOfCoefficients];
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
  MatrixComplexReference<T> result(numberOfColumns(), numberOfRows());
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
  return result;
}

template<typename T>
MatrixComplexReference<T>::MatrixComplexReference(int numberOfRows, int numberOfColumns) :
  EvaluationReference<T>()
{
  TreeNode * node = TreePool::sharedPool()->createTreeNode<MatrixComplexNode<T>>();
  this->m_identifier = node->identifier();
  if (!(node->isAllocationFailure())) {
    static_cast<MatrixComplexNode<T> *>(node)->setMatrixComplexDimension(numberOfRows, numberOfColumns);
  }
}

template<typename T>
MatrixComplexReference<T>::MatrixComplexReference(std::complex<T> * operands, int numberOfRows, int numberOfColumns) :
  MatrixComplexReference<T>(numberOfRows, numberOfColumns)
{
  for (int i=0; i<numberOfRows*numberOfColumns; i++) {
    this->addChildTreeAtIndex(ComplexReference<T>(operands[i]), i, i);
  }
}

template<typename T>
MatrixComplexReference<T> MatrixComplexReference<T>::createIdentity(int dim) {
  MatrixComplexReference<T> result(dim, dim);
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      ComplexReference<T> c = i == j ? ComplexReference<T>(1.0) : ComplexReference<T>(0.0);
      result.addChildTreeAtIndex(c, i*dim+j, i*dim+j);
    }
  }
  return result;
}

template<typename T>
int MatrixComplexReference<T>::numberOfRows() const {
  if (this->node()->isAllocationFailure()) {
    return 0;
  }
  return this->typedNode()->numberOfRows();
}

template<typename T>
int MatrixComplexReference<T>::numberOfColumns() const {
  if (this->node()->isAllocationFailure()) {
    return 0;
  }
  return this->typedNode()->numberOfColumns();
}

template class MatrixComplexReference<float>;
template class MatrixComplexReference<double>;

}
