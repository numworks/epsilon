#include <poincare/matrix.h>
#include <poincare/division.h>
#include <poincare/matrix_complex.h>
#include <poincare/matrix_layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
#include <cmath>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

namespace Poincare {

void MatrixNode::didAddChildAtIndex(int newNumberOfChildren) {
  setNumberOfRows(1);
  setNumberOfColumns(newNumberOfChildren);
}

int MatrixNode::polynomialDegree(Context * context, const char * symbolName) const {
  return -1;
}

Layout MatrixNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(numberOfChildren() > 0);
  MatrixLayout layout = MatrixLayout::Builder();
  for (ExpressionNode * c : children()) {
    layout.addChildAtIndex(c->createLayout(floatDisplayMode, numberOfSignificantDigits), layout.numberOfChildren(), layout.numberOfChildren(), nullptr);
  }
  layout.setDimensions(m_numberOfRows, m_numberOfColumns);
  return layout;
}

int MatrixNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) {
    return 0;
  }
  currentChar += SerializationHelper::CodePoint(buffer + currentChar, bufferSize - currentChar, '[');
  if (currentChar >= bufferSize-1) {
    return currentChar;
  }
  for (int i = 0; i < m_numberOfRows; i++) {
    currentChar += SerializationHelper::CodePoint(buffer + currentChar, bufferSize - currentChar, '[');
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    currentChar += childAtIndex(i*m_numberOfColumns)->serialize(buffer+currentChar, bufferSize-currentChar, floatDisplayMode, numberOfSignificantDigits);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    for (int j = 1; j < m_numberOfColumns; j++) {
      currentChar += SerializationHelper::CodePoint(buffer + currentChar, bufferSize - currentChar, ',');
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
      currentChar += childAtIndex(i*m_numberOfColumns+j)->serialize(buffer+currentChar, bufferSize-currentChar, floatDisplayMode, numberOfSignificantDigits);
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    currentChar += SerializationHelper::CodePoint(buffer + currentChar, bufferSize - currentChar, ']');
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
  }
  currentChar += SerializationHelper::CodePoint(buffer + currentChar, bufferSize - currentChar, ']');
  if (currentChar >= bufferSize-1) {
    return currentChar;
  }
  buffer[currentChar] = 0;
  return currentChar;
}

template<typename T>
Evaluation<T> MatrixNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (ExpressionNode * c : children()) {
    matrix.addChildAtIndexInPlace(c->approximate(T(), context, complexFormat, angleUnit), matrix.numberOfChildren(), matrix.numberOfChildren());
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return matrix;
}

// MATRIX

void Matrix::setDimensions(int rows, int columns) {
  assert(rows * columns == numberOfChildren());
  setNumberOfRows(rows);
  setNumberOfColumns(columns);
}

void Matrix::addChildrenAsRowInPlace(TreeHandle t, int i) {
  int previousNumberOfColumns = numberOfColumns();
  if (previousNumberOfColumns > 0) {
    assert(t.numberOfChildren() == numberOfColumns());
  }
  int previousNumberOfRows = numberOfRows();
  for (int i = 0; i < t.numberOfChildren(); i++) {
    addChildAtIndexInPlace(t.childAtIndex(i), numberOfChildren(), numberOfChildren());
  }
  setDimensions(previousNumberOfRows + 1, previousNumberOfColumns == 0 ? t.numberOfChildren() : previousNumberOfColumns);
}

int Matrix::rank(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool inPlace) {
  Matrix m = inPlace ? *this : clone().convert<Matrix>();
  m = m.rowCanonize(context, complexFormat, angleUnit);
  int rank = m.numberOfRows();
  int i = rank-1;
  while (i >= 0) {
    int j = m.numberOfColumns()-1;
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
int Matrix::ArrayInverse(T * array, int numberOfRows, int numberOfColumns) {
  if (numberOfRows != numberOfColumns) {
    return -1;
  }
  assert(numberOfRows*numberOfColumns <= k_maxNumberOfCoefficients);
  int dim = numberOfRows;
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix
   * (A is squared with less than k_maxNumberOfCoefficients so (A|I) has less than
   * 2*k_maxNumberOfCoefficients */
  T operands[2*k_maxNumberOfCoefficients];
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
    if (std::abs(operands[i*2*dim+i] - (T)1.0) > Expression::Epsilon<float>()) {
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

Matrix Matrix::rowCanonize(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Multiplication determinant) {
  Expression::SetInterruption(false);
  // The matrix children have to be reduced to be able to spot 0
  ExpressionNode::ReductionContext systemReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::System);
  deepReduceChildren(systemReductionContext);

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
      if (!determinant.isUninitialized()) { determinant.addChildAtIndexInPlace(Rational::Builder(0), 0, determinant.numberOfChildren()); }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          swapChildrenInPlace(iPivot*n+col, h*n+col);
        }
        // Update determinant: det *= -1
        if (!determinant.isUninitialized()) {
          determinant.addChildAtIndexInPlace(Rational::Builder(-1), 0, determinant.numberOfChildren());
        }
      }
      /* Set to 1 M[h][k] by linear combination */
      Expression divisor = matrixChild(h, k);
      // Update determinant: det *= divisor
      if (!determinant.isUninitialized()) {
        determinant.addChildAtIndexInPlace(divisor.clone(), 0, determinant.numberOfChildren());
      }
      for (int j = k+1; j < n; j++) {
        Expression opHJ = matrixChild(h, j);
        Expression newOpHJ = Division::Builder(opHJ, divisor.clone());
        replaceChildAtIndexInPlace(h*n+j, newOpHJ);
        newOpHJ = newOpHJ.shallowReduce(systemReductionContext);
      }
      replaceChildInPlace(divisor, Rational::Builder(1));

      /* Set to 0 all M[i][j] i != h, j > k by linear combination */
      for (int i = 0; i < m; i++) {
        if (i == h) { continue; }
        Expression factor = matrixChild(i, k);
        for (int j = k+1; j < n; j++) {
          Expression opIJ = matrixChild(i, j);
          Expression newOpIJ = Subtraction::Builder(opIJ, Multiplication::Builder(matrixChild(h, j).clone(), factor.clone()));
          replaceChildAtIndexInPlace(i*n+j, newOpIJ);
          newOpIJ.childAtIndex(1).shallowReduce(systemReductionContext);
          newOpIJ = newOpIJ.shallowReduce(systemReductionContext);
        }
        replaceChildAtIndexInPlace(i*n+k, Rational::Builder(0));
      }
      h++;
      k++;
    }
  }
  return *this;
}

template<typename T>
void Matrix::ArrayRowCanonize(T * array, int numberOfRows, int numberOfColumns, T * determinant) {
  int h = 0; // row pivot
  int k = 0; // column pivot

  while (h < numberOfRows && k < numberOfColumns) {
    // Find the first non-null pivot
    int iPivot = h;
    while (iPivot < numberOfRows && std::abs(array[iPivot*numberOfColumns+k]) < Expression::Epsilon<double>()) {
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

#if 0
#if MATRIX_EXACT_REDUCING
Matrix Matrix::transpose() const {
  Matrix matrix();
  for (int i = 0; i < m_numberOfRows; i++) {
    for (int j = 0; j < m_numberOfColumns; j++) {
      matrix.addChildAtIndexInPlace(childAtIndex(i*m_numberOfRows+j), j*m_numberOfRows+i, j*m_numberOfRows+i);
    }
  }
  // Intentionally swapping dimensions for transpose
  matrix.setDimensions(m_numberOfColumns, m_numberOfRows);
  return matrix;
}

Matrix Matrix::CreateIdentity(int dim) {
  Matrix matrix();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      matrix.addChildAtIndexInPlace(i == j ? Rational::Builder(1) : Rational::Builder(0), i*dim+j, i*dim+j);
    }
  }
  matrix.setDimensions(dim, dim);
  return matrix;
}

Expression Matrix::inverse(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (m_numberOfRows != m_numberOfColumns) {
    return Undefined::Builder();
  }
  int dim = m_numberOfRows;
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  Matrix AI = Matrix::Builder();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      AI.addChildAtIndexInPlace(matrixChild(i, j), i*2*dim+j, i*2*dim+j);
    }
    for (int j = dim; j < 2*dim; j++) {
      AI.addChildAtIndexInPlace(j-dim == i ? Rational::Builder(1) : Rational::Builder(0), i*2*dim+j, i*2*dim+j);
    }
  }
  AI.setDimensions(dim, 2*dim);
  AI = AI.rowCanonize(context, complexFormat, angleUnit);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (AI.matrixChild(i, i)->type() != ExpressionNode::Type::Rational || !static_cast<RationalNode *>(AI.matrixChild(i, i)->node())->isOne()) {
      return Undefined::Builder();
    }
  }
  Matrix inverse();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      addChildAtIndexInPlace(AI.matrixChild(i, j+dim), i*dim+j, i*dim+j);
    }
  }
  inverse.setDimensions(dim, dim);
  return inverse;
}

#endif
#endif

template int Matrix::ArrayInverse<float>(float *, int, int);
template int Matrix::ArrayInverse<double>(double *, int, int);
template int Matrix::ArrayInverse<std::complex<float>>(std::complex<float> *, int, int);
template int Matrix::ArrayInverse<std::complex<double>>(std::complex<double> *, int, int);
template void Matrix::ArrayRowCanonize<std::complex<float> >(std::complex<float>*, int, int, std::complex<float>*);
template void Matrix::ArrayRowCanonize<std::complex<double> >(std::complex<double>*, int, int, std::complex<double>*);

}
