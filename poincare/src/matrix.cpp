extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/matrix_layout_node.h>
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

int MatrixNode::polynomialDegree(char symbolName) const {
  return -1;
}

LayoutRef MatrixNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  MatrixLayoutRef layout;
  /* To keep track of the number of children, number of columns is set to 1 and
   * number of rows is incremented with each child addition. The real number of
   * rows and columns is properly set once all children are added. */
  layout.setNumberOfRows(0);
  layout.setNumberOfColumns(1);
  LayoutRef castedLayout(layout.node());
  for (int i = 0; i < numberOfChildren(); i++) {
    castedLayout.addChildAtIndex(childAtIndex(i)->createLayout(floatDisplayMode, numberOfSignificantDigits), i, i, nullptr);
    layout.setNumberOfRows(i+1);
  }
  layout.setNumberOfRows(m_numberOfRows);
  layout.setNumberOfColumns(m_numberOfColumns);
  return layout;
}

int MatrixNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) {
    return 0;
  }
  buffer[currentChar++] = '[';
  if (currentChar >= bufferSize-1) {
    return currentChar;
  }
  for (int i = 0; i < m_numberOfRows; i++) {
    buffer[currentChar++] = '[';
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    currentChar += childAtIndex(i*m_numberOfColumns)->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, floatDisplayMode, numberOfSignificantDigits);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    for (int j = 1; j < m_numberOfColumns; j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
      currentChar += childAtIndex(i*m_numberOfColumns+j)->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, floatDisplayMode, numberOfSignificantDigits);
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    buffer[currentChar++] = ']';
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
  }
  buffer[currentChar++] = ']';
  buffer[currentChar] = 0;
  return currentChar;
}

template<typename T>
Evaluation<T> MatrixNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  MatrixComplexReference<T> matrix;
  for (int i = 0; i < numberOfChildren(); i++) {
    Evaluation<T> operandEvaluation = childAtIndex(i)->approximate(T(), context, angleUnit);
    if (operandEvaluation.node()->type() != Evaluation<T>::Type::Complex) {
      result.addChildTreeAtIndex(ComplexReference<T>::Undefined(), i, i);
    } else {
      result.addChildTreeAtIndex(compute(*child, angleUnit), i, i);
    }
  }
  matrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  return matrix;
}

// MATRIX REFERENCE

void MatrixReference::setDimensions(int rows, int columns) {
  if (isAllocationFailure()) {
    return;
  }
  assert(rows * columns = numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

int MatrixReference::numberOfRows() const {
  if (isAllocationFailure()) {
    return 0;
  }
  return typedNode()->numberOfRows();
}

int MatrixReference::numberOfColumns() const {
  if (isAllocationFailure()) {
    return 0;
  }
  return typedNode()->numberOfColumns();
}

void MatrixReference::addChildTreeAtIndex(TreeReference t, int index, int currentNumberOfChildren) {
  ExpressionReference::addChildTreeAtIndex(t, index, currentNumberOfChildren);
  if (isAllocationFailure()) {
    return;
  }
  setNumberOfRows(1);
  setNumberOfColumns(currentNumberOfChildren + 1);
}

int MatrixReference::rank(Context & context, Preferences::AngleUnit angleUnit, bool inPlace) {
  MatrixReference m = inPlace ? MatrixReference(this) : MatrixReference(clone().node());
  m.rowCanonize(context, angleUnit);
  int rank = m.numberOfRows();
  int i = rank-1;
  while (i >= 0) {
    int j = m.numberOfColumns()-1;
    ExpressionReference child = matrixChild(i,j);
    if (child.isAllocationFailure()) {
      return 0;
    }
    while (j >= i && matrixChild(i,j).isRationalZero()) {
      j--;
    }
    if (j == i-1) {
      rank--;
    } else {
      break;
    }
    i--;
  }
  return rank;
}

template<typename T>
int MatrixReference::ArrayInverse(T * array, int numberOfRows, int numberOfColumns) {
  if (numberOfRows != numberOfColumns) {
    return -1;
  }
  assert(numberOfRows*numberOfColumns <= k_maxNumberOfCoefficients);
  int dim = numberOfRows;
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  T operands[k_maxNumberOfCoefficients*(3/2)]; // inv dimensions: dim x (2*dim)
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*2*dim+j] = array[i*numberOfColumns+j];
    }
    for (int j = dim; j < 2*dim; j++) {
      operands[i*2*dim+j] = j-dim == i ? 1.0 : 0.0;
    }
  }
  ArrayRowCanonize(operands, dim, 2*dim);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (std::abs(operands[i*2*dim+i] - (T)1.0) > Expression::epsilon<float>()) {
      return -2;
    }
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      array[i*numberOfColumns+j] = operands[i*2*dim+j+dim];
    }
  }
  return 0;
}

void MatrixReference::setNumberOfRows(int rows) {
  if (isAllocationFailure()) {
    return;
  }
  assert(rows >= 0);
  typedNode()->setNumberOfRows(rows);
}

void MatrixReference::setNumberOfColumns(int columns) {
  if (isAllocationFailure()) {
    return;
  }
  assert(columns >= 0);
  typedNode()->setNumberOfColumns(columns);
}

void MatrixReference::rowCanonize(Context & context, Preferences::AngleUnit angleUnit, MultiplicationReference determinant) {
  // The matrix has to be reduced to be able to spot 0 inside it
  ExpressionReference reduced = deepReduce(context, angleUnit);
  /* The MatrixNode should change only in 2 cases:
   * - Allocation failure
   * - One of the child is Undefined
   */
  if (reduced != *this) {
    return;
  }

  int m = numberOfRows();
  int n = numberOfColumns();

  int h = 0; // row pivot
  int k = 0; // column pivot

  while (h < m && k < n) {
    // Find the first non-null pivot
    int iPivot = h;
    while (iPivot < m && matrixChild(iPivot, k).isRationalZero()) {
      iPivot++;
    }
    if (iPivot == m) {
      // No non-null coefficient in this column, skip
      k++;
      // Update determinant: det *= 0
      if (determinant) { determinant.addChildTreeAtIndex(RationalReference(0), 0, determinant.numberOfChildren()); }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          swapChildren(iPivot*n+col, h*n+col);
        }
        // Update determinant: det *= -1
        if (determinant) { determinant.addChildTreeAtIndex(RationalReference(-1), 0, determinant.numberOfChildren()); }
      }
      /* Set to 1 M[h][k] by linear combination */
      ExpressionReference divisor = matrixChild(h, k);
      // Update determinant: det *= divisor
      if (determinant) { determinant.addChildTreeAtIndex(divisor.clone()); }
      for (int j = k+1; j < n; j++) {
        ExpressionReference opHJ = matrixChild(h, j);
        ExpressionReference newOpHJ = DivisionReference(opHJ, divisor.clone());
        replaceTreeChildAtIndex(h*n+j, newOpHJ);
        newOpHJ.node()->shallowReduce(context, angleUnit);
      }
      matrixChild(h, k).replaceWith(RationalReference(1));

      /* Set to 0 all M[i][j] i != h, j > k by linear combination */
      for (int i = 0; i < m; i++) {
        if (i == h) { continue; }
        ExpressionReference factor = matrixChild(i, k);
        for (int j = k+1; j < n; j++) {
          ExpressionReference opIJ = matrixChild(i, j);
          ExpressionReference newOpIJ = SubtractionReference(opIJ, MultiplicationReference(matrixChild(h, j).clone(), factor.clone()));
          replaceTreeChildAtIndex(i*n+j, newOpIJ);
          newOpIJ.childAtIndex(1)->node()->shallowReduce(context, angleUnit);
          newOpIJ.node()->shallowReduce(context, angleUnit);
        }
        matrixChild(i, k).replaceWith(RationalReference(0));
      }
      h++;
      k++;
    }
  }
}

template<typename T>
void MatrixReference::ArrayRowCanonize(T * array, int numberOfRows, int numberOfColumns, T * determinant) {
  int h = 0; // row pivot
  int k = 0; // column pivot

  while (h < numberOfRows && k < numberOfColumns) {
    // Find the first non-null pivot
    int iPivot = h;
    while (iPivot < numberOfRows && std::abs(array[iPivot*numberOfColumns+k]) < Expression::epsilon<double>()) {
      iPivot++;
    }
    if (iPivot == numberOfRows) {
      // No non-null coefficient in this column, skip
      k++;
      // Update determinant: det *= 0
      if (determinant) { *determinant *= 0.0; }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < numberOfColumns; col++) {
          // Swap array[iPivot, col] and array[h, col]
          T temp = array[iPivot*numberOfColumns+col];
          array[iPivot*numberOfColumns+col] = array[h*numberOfColumns+col];
          array[h*numberOfColumns+col] = temp;
        }
        // Update determinant: det *= -1
        if (determinant) { *determinant *= -1.0; }
      }
      /* Set to 1 array[h][k] by linear combination */
      T divisor = array[h*numberOfColumns+k];
      // Update determinant: det *= divisor
      if (determinant) { *determinant *= divisor; }
      for (int j = k+1; j < numberOfColumns; j++) {
        array[h*numberOfColumns+j] /= divisor;
      }
      array[h*numberOfColumns+k] = 1;

      /* Set to 0 all M[i][j] i != h, j > k by linear combination */
      for (int i = 0; i < numberOfRows; i++) {
        if (i == h) { continue; }
        T factor = array[i*numberOfColumns+k];
        for (int j = k+1; j < numberOfColumns; j++) {
          array[i*numberOfColumns+j] -= array[h*numberOfColumns+j]*factor;
        }
        array[i*numberOfColumns+k] = 0;
      }
      h++;
      k++;
    }
  }
}

#if MATRIX_EXACT_REDUCING
MatrixReference MatrixReference::transpose() const {
  MatrixReference matrix();
  for (int i = 0; i < m_numberOfRows; i++) {
    for (int j = 0; j < m_numberOfColumns; j++) {
      addChildTreeAtIndex(childAtIndex(i*m_numberOfRows+j).clone(), j*m_numberOfRows+i, j*m_numberOfRows+i);
    }
  }
  // Intentionally swapping dimensions for transpose
  matrix.setDimensions(m_numberOfColumns, m_numberOfRows);
  return matrix;
}

Matrix * MatrixReference::createIdentity(int dim) {
  MatrixReference matrix();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        addChildTreeAtIndex(childAtIndex(RationalReference(1), i*dim+j, i*dim+j);
      } else {
        addChildTreeAtIndex(childAtIndex(RationalReference(0), i*dim+j, i*dim+j);
      }
    }
  }
  matrix.setDimensions(dim, dim);
  return matrix;
}

Expression * MatrixReference::inverse(Context & context, Preferences::AngleUnit angleUnit) const {
  if (m_numberOfRows != m_numberOfColumns) {
    return UndefinedReference();
  }
  int dim = m_numberOfRows;
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  MatrixReference AI;
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      addChildTreeAtIndex(matrixChild(i, j).clone(), i*2*dim+j, i*2*dim+j);
    }
    for (int j = dim; j < 2*dim; j++) {
      addChildTreeAtIndex(j-dim == i ? RationalReference(1) : RationalReference(0), i*2*dim+j, i*2*dim+j);
    }
  }
  AI.setDimensions(dim, 2*dim);
  AI.rowCanonize(context, angleUnit);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (AI.matrixChild(i, i)->type() != Type::Rational || !static_cast<RationalNode *>(AI.matrixChild(i, i)->node())->isOne()) {
      return UndefinedReference();
    }
  }
  MatrixReference inverse();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      addChildTreeAtIndex(AI.matrixChild(i, j+dim), i*dim+j, i*dim+j);
    }
  }
  inverse.setDimensions(dim, dim);
  return inverse;
}

#endif

template int MatrixReference::ArrayInverse<float>(float *, int, int);
template int MatrixReference::ArrayInverse<double>(double *, int, int);
template int MatrixReference::ArrayInverse<std::complex<float>>(std::complex<float> *, int, int);
template int MatrixReference::ArrayInverse<std::complex<double>>(std::complex<double> *, int, int);
template void MatrixReference::ArrayRowCanonize<std::complex<float> >(std::complex<float>*, int, int, std::complex<float>*);
template void MatrixReference::ArrayRowCanonize<std::complex<double> >(std::complex<double>*, int, int, std::complex<double>*);

}
