#include <assert.h>
#include <float.h>
#include <ion.h>
#include <poincare/old/matrix.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/old_expression.h>
#include <poincare/old/undefined.h>

#include <cmath>
#include <utility>

namespace Poincare {

template <typename T>
std::complex<T> MatrixComplexNode<T>::complexAtIndex(int index) const {
  EvaluationNode<T> *child = EvaluationNode<T>::childAtIndex(index);
  if (child->otype() == EvaluationNode<T>::Type::Complex) {
    return *(static_cast<ComplexNode<T> *>(child));
  }
  return std::complex<T>(NAN, NAN);
}

template <typename T>
OExpression MatrixComplexNode<T>::complexToExpression(
    Preferences::ComplexFormat complexFormat) const {
  if (isUndefined()) {
    return Undefined::Builder();
  }
  OMatrix matrix = OMatrix::Builder();
  int i = 0;
  for (EvaluationNode<T> *c : this->children()) {
    OExpression childExpression = Undefined::Builder();
    if (c->otype() == EvaluationNode<T>::Type::Complex) {
      childExpression = c->complexToExpression(complexFormat);
    }
    matrix.addChildAtIndexInPlace(childExpression, i, i);
    i++;
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return std::move(matrix);
}

template <typename T>
std::complex<T> MatrixComplexNode<T>::trace() const {
  if (numberOfRows() != numberOfColumns() || numberOfRows() == 0) {
    return std::complex<T>(NAN, NAN);
  }
  int dim = numberOfRows();
  std::complex<T> c = std::complex<T>(0);
  for (int i = 0; i < dim; i++) {
    c += complexAtIndex(i * dim + i);
    if (std::isnan(c.real()) || std::isnan(c.imag())) {
      return std::complex<T>(NAN, NAN);
    }
  }
  return c;
}

template <typename T>
std::complex<T> MatrixComplexNode<T>::determinant() const {
  if (numberOfRows() != numberOfColumns() || numberOfChildren() == 0 ||
      numberOfChildren() > OMatrix::k_maxNumberOfChildren) {
    return std::complex<T>(NAN, NAN);
  }
  std::complex<T> operandsCopy[OMatrix::k_maxNumberOfChildren];
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    // Returns complex<T>(NAN, NAN) if Node type is not Complex
    operandsCopy[i] = complexAtIndex(i);
  }
  std::complex<T> determinant = std::complex<T>(1);
  OMatrix::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns,
                            &determinant);
  return determinant;
}

template <typename T>
MatrixComplex<T> MatrixComplexNode<T>::inverse() const {
  if (numberOfRows() != numberOfColumns() || numberOfChildren() == 0 ||
      numberOfChildren() > OMatrix::k_maxNumberOfChildren) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[OMatrix::k_maxNumberOfChildren];
  int i = 0;
  for (EvaluationNode<T> *c : this->children()) {
    if (c->otype() != EvaluationNode<T>::Type::Complex) {
      return MatrixComplex<T>::Undefined();
    }
    operandsCopy[i] = complexAtIndex(i);
    i++;
  }
  int result =
      OMatrix::ArrayInverse(operandsCopy, m_numberOfRows, m_numberOfColumns);
  if (result == 0) {
    /* Intentionally swapping dimensions for inverse, although it doesn't make a
     * difference because it is square. */
    return MatrixComplex<T>::Builder(operandsCopy, m_numberOfColumns,
                                     m_numberOfRows);
  }
  return MatrixComplex<T>::Undefined();
}

template <typename T>
MatrixComplex<T> MatrixComplexNode<T>::transpose() const {
  if (isUndefined()) {
    return MatrixComplex<T>::Undefined();
  }
  // Intentionally swapping dimensions for transpose
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int j = 0; j < numberOfColumns(); j++) {
    for (int i = 0; i < numberOfRows(); i++) {
      result.addChildAtIndexInPlace(
          Complex<T>::Builder(complexAtIndex(i * numberOfColumns() + j)),
          result.numberOfChildren(), result.numberOfChildren());
    }
  }
  result.setDimensions(numberOfColumns(), numberOfRows());
  return result;
}

template <typename T>
MatrixComplex<T> MatrixComplexNode<T>::ref(bool reduced) const {
  // Compute OMatrix Row Echelon Form
  if (numberOfChildren() == 0 ||
      numberOfChildren() > OMatrix::k_maxNumberOfChildren) {
    return MatrixComplex<T>::Undefined();
  }
  std::complex<T> operandsCopy[OMatrix::k_maxNumberOfChildren];
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    // Returns complex<T>(NAN, NAN) if Node type is not Complex
    operandsCopy[i] = complexAtIndex(i);
  }
  /* Reduced row echelon form is also called row canonical form. To compute the
   * row echelon form (non reduced one), fewer steps are required. */
  OMatrix::ArrayRowCanonize(operandsCopy, m_numberOfRows, m_numberOfColumns,
                            static_cast<std::complex<T> *>(nullptr), reduced);
  return MatrixComplex<T>::Builder(operandsCopy, m_numberOfRows,
                                   m_numberOfColumns);
}

// MATRIX COMPLEX REFERENCE

template <typename T>
MatrixComplex<T> MatrixComplex<T>::Builder(std::complex<T> *operands,
                                           int numberOfRows,
                                           int numberOfColumns) {
  MatrixComplex<T> m = MatrixComplex<T>::Builder();
  for (int i = 0; i < numberOfRows * numberOfColumns; i++) {
    m.addChildAtIndexInPlace(Complex<T>::Builder(operands[i]), i, i);
  }
  m.setDimensions(numberOfRows, numberOfColumns);
  return m;
}

template <typename T>
MatrixComplex<T> MatrixComplex<T>::Undefined() {
  MatrixComplex<T> m = MatrixComplex<T>::Builder();
  m.setDimensions(-1, 0);
  return m;
}

template <typename T>
void MatrixComplex<T>::setDimensions(int rows, int columns) {
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

template <typename T>
void MatrixComplex<T>::addChildAtIndexInPlace(Evaluation<T> t, int index,
                                              int currentNumberOfChildren) {
  if (t.otype() != EvaluationNode<T>::Type::Complex) {
    t = Complex<T>::Undefined();
  }
  Evaluation<T>::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
}

template class MatrixComplexNode<float>;
template class MatrixComplexNode<double>;

template class MatrixComplex<float>;
template class MatrixComplex<double>;

}  // namespace Poincare
