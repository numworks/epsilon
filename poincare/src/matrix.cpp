#include <poincare/matrix.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/matrix_complex.h>
#include <poincare/matrix_layout.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>
#include <assert.h>
#include <cmath>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <utility>

namespace Poincare {

static inline int minInt(int x, int y) { return x < y ? x : y; }

bool MatrixNode::hasMatrixChild(Context * context) const {
  for (ExpressionNode * c : children()) {
    if (Expression(c).deepIsMatrix(context)) {
      return true;
    }
  }
  return false;
}

void MatrixNode::didAddChildAtIndex(int newNumberOfChildren) {
  setNumberOfRows(1);
  setNumberOfColumns(newNumberOfChildren);
}

int MatrixNode::polynomialDegree(Context * context, const char * symbolName) const {
  return -1;
}

Expression MatrixNode::shallowReduce(ReductionContext reductionContext) {
  return Matrix(this).shallowReduce(reductionContext.context());
}

Layout MatrixNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(numberOfChildren() > 0);
  MatrixLayout layout = MatrixLayout::Builder();
  for (ExpressionNode * c : children()) {
    layout.addChildAtIndex(c->createLayout(floatDisplayMode, numberOfSignificantDigits), layout.numberOfChildren(), layout.numberOfChildren(), nullptr);
  }
  layout.setDimensions(m_numberOfRows, m_numberOfColumns);
  return std::move(layout);
}

int MatrixNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) {
    return 0;
  }
  int currentChar = SerializationHelper::CodePoint(buffer, bufferSize, '[');
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
  return minInt(currentChar, bufferSize-1);
}

template<typename T>
Evaluation<T> MatrixNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  MatrixComplex<T> matrix = MatrixComplex<T>::Builder();
  for (ExpressionNode * c : children()) {
    matrix.addChildAtIndexInPlace(c->approximate(T(), context, complexFormat, angleUnit), matrix.numberOfChildren(), matrix.numberOfChildren());
  }
  matrix.setDimensions(numberOfRows(), numberOfColumns());
  return std::move(matrix);
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
  ExpressionNode::ReductionContext systemReductionContext = ExpressionNode::ReductionContext(context, complexFormat, angleUnit, ExpressionNode::ReductionTarget::SystemForApproximation);
  m = m.rowCanonize(systemReductionContext, nullptr);
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

Matrix Matrix::rowCanonize(ExpressionNode::ReductionContext reductionContext, Expression * determinant) {
  Expression::SetInterruption(false);
  // The matrix children have to be reduced to be able to spot 0
  deepReduceChildren(reductionContext);

  Multiplication det = Multiplication::Builder();

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
      if (determinant) {
        // Update determinant: det *= 0
        det.addChildAtIndexInPlace(Rational::Builder(0), det.numberOfChildren(), det.numberOfChildren());
      }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          swapChildrenInPlace(iPivot*n+col, h*n+col);
        }
        if (determinant) {
          // Update determinant: det *= -1
          det.addChildAtIndexInPlace(Rational::Builder(-1), det.numberOfChildren(), det.numberOfChildren());
        }
      }
      // Set to 1 M[h][k] by linear combination
      Expression divisor = matrixChild(h, k);
      // Update determinant: det *= divisor
      if (determinant) {
        det.addChildAtIndexInPlace(divisor.clone(), det.numberOfChildren(), det.numberOfChildren());
      }
      for (int j = k+1; j < n; j++) {
        Expression opHJ = matrixChild(h, j);
        Expression newOpHJ = Division::Builder(opHJ, divisor.clone());
        replaceChildAtIndexInPlace(h*n+j, newOpHJ);
        newOpHJ = newOpHJ.shallowReduce(reductionContext);
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
          newOpIJ.childAtIndex(1).shallowReduce(reductionContext);
          newOpIJ = newOpIJ.shallowReduce(reductionContext);
        }
        replaceChildAtIndexInPlace(i*n+k, Rational::Builder(0));
      }
      h++;
      k++;
    }
  }
  if (determinant) {
    *determinant = det;
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
      // Set to 1 array[h][k] by linear combination
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

Matrix Matrix::CreateIdentity(int dim) {
  Matrix matrix = Matrix::Builder();
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      matrix.addChildAtIndexInPlace(i == j ? Rational::Builder(1) : Rational::Builder(0), i*dim+j, i*dim+j);
    }
  }
  matrix.setDimensions(dim, dim);
  return matrix;
}


Matrix Matrix::createTranspose() const {
  Matrix matrix = Matrix::Builder();
  for (int j = 0; j < numberOfColumns(); j++) {
    for (int i = 0; i < numberOfRows(); i++) {
      matrix.addChildAtIndexInPlace(const_cast<Matrix *>(this)->matrixChild(i, j).clone(), matrix.numberOfChildren(), matrix.numberOfChildren());
    }
  }
  // Intentionally swapping dimensions for transpose
  matrix.setDimensions(numberOfColumns(), numberOfRows());
  return matrix;
}

Expression Matrix::createInverse(ExpressionNode::ReductionContext reductionContext, bool * couldComputeInverse) const {
  int dim = numberOfRows();
  if (dim != numberOfColumns()) {
    *couldComputeInverse = true;
    return Undefined::Builder();
  }
  Expression result = computeInverseOrDeterminant(false, reductionContext, couldComputeInverse);
  assert(!(*couldComputeInverse) || !result.isUninitialized());
  return result;
}

Expression Matrix::determinant(ExpressionNode::ReductionContext reductionContext, bool * couldComputeDeterminant, bool inPlace) {
  *couldComputeDeterminant = true;
  Matrix m = inPlace ? *this : clone().convert<Matrix>();
  int dim = m.numberOfRows();
  if (dim != m.numberOfColumns()) {
    // Determinant is for square matrices
    return Undefined::Builder();
  }
  // Explicit formulas for dimensions from 1 to 3
  if (dim == 1) {
    // Determinant of [[a]] is a
    return m.childAtIndex(0);
  }
  if (dim == 2) {
    /*                |a b|
     * Determinant of |c d| is ad-bc   */
    Multiplication ad = Multiplication::Builder(m.matrixChild(0,0), m.matrixChild(1,1));
    Multiplication bc = Multiplication::Builder(m.matrixChild(0,1), m.matrixChild(1,0));
    Expression result = Subtraction::Builder(ad, bc);
    ad.shallowReduce(reductionContext);
    bc.shallowReduce(reductionContext);
    return result;
  }
  if (dim == 3) {
    /*                |a b c|
     * Determinant of |d e f| is aei+bfg+cdh-ceg-bdi-afh
     *                |g h i|                             */
    Expression a = m.matrixChild(0,0);
    Expression b = m.matrixChild(0,1);
    Expression c = m.matrixChild(0,2);
    Expression d = m.matrixChild(1,0);
    Expression e = m.matrixChild(1,1);
    Expression f = m.matrixChild(1,2);
    Expression g = m.matrixChild(2,0);
    Expression h = m.matrixChild(2,1);
    Expression i = m.matrixChild(2,2);
    constexpr int additionChildrenCount = 6;
    Expression additionChildren[additionChildrenCount] = {
      Multiplication::Builder(a.clone(), e.clone(), i.clone()),
      Multiplication::Builder(b.clone(), f.clone(), g.clone()),
      Multiplication::Builder(c.clone(), d.clone(), h.clone()),
      Multiplication::Builder(Rational::Builder(-1), c, e, g),
      Multiplication::Builder(Rational::Builder(-1), b, d, i),
      Multiplication::Builder(Rational::Builder(-1), a, f, h)};
    Expression result = Addition::Builder(additionChildren, additionChildrenCount);
    for (int i = 0; i < additionChildrenCount; i++) {
      additionChildren[i].shallowReduce(reductionContext);
    }
    return result;
  }

  // Dimension >= 4
  Expression result = computeInverseOrDeterminant(true, reductionContext, couldComputeDeterminant);
  assert(!(*couldComputeDeterminant) || !result.isUninitialized());
  return result;
}

Expression Matrix::shallowReduce(Context * context) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  if (node()->hasMatrixChild(context)) {
    return replaceWithUndefinedInPlace();
  }
  return *this;
}

Expression Matrix::computeInverseOrDeterminant(bool computeDeterminant, ExpressionNode::ReductionContext reductionContext, bool * couldCompute) const {
  assert(numberOfRows() == numberOfColumns());
  int dim = numberOfRows();
  /* If the matrix is too big, the rowCanonization might not be computed exactly
   * because of a pool allocation error, but we might still be able to compute
   * it approximately. We thus encapsulate the inverse/determinant creation in
   * an exception checkpoint.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there is no Store
   * in the matrix. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    /* We clone the current matrix to extract its children later. We can't clone
     * its children directly. Indeed, the current matrix node (this->node()) is
     * located before the exception checkpoint. In order to clone its chidlren,
     * we would temporary increase the reference counter of each child (also
     * located before the checkpoint). If an exception is raised before
     * destroying the child handle, its reference counter would be off by one
     * after the long jump. */
    Matrix cl = clone().convert<Matrix>();
    *couldCompute = true;
    /* Create the matrix (A|I) with A is the input matrix and I the dim
     * identity matrix */
    Matrix matrixAI = Matrix::Builder();
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        Expression mChildIJ = cl.matrixChild(i, j);
        matrixAI.addChildAtIndexInPlace(mChildIJ, i*2*dim+j, i*2*dim+j);
      }
      for (int j = dim; j < 2*dim; j++) {
        matrixAI.addChildAtIndexInPlace(j-dim == i ? Rational::Builder(1) : Rational::Builder(0), i*2*dim+j, i*2*dim+j);
      }
    }
    matrixAI.setDimensions(dim, 2*dim);
    if (computeDeterminant) {
      // Compute the determinant
      Expression d;
      matrixAI.rowCanonize(reductionContext, &d);
      return d;
    }
    // Compute the inverse
    matrixAI = matrixAI.rowCanonize(reductionContext, nullptr);
    // Check inversibility
    for (int i = 0; i < dim; i++) {
      if (!matrixAI.matrixChild(i, i).isRationalOne()) {
        return Undefined::Builder();
      }
    }
    Matrix inverse = Matrix::Builder();
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        inverse.addChildAtIndexInPlace(matrixAI.matrixChild(i, j+dim), i*dim+j, i*dim+j); // We can steal matrixAI's children
      }
    }
    inverse.setDimensions(dim, dim);
    return std::move(inverse);
  } else {
    *couldCompute = false;
    return Expression();
  }
}


template int Matrix::ArrayInverse<float>(float *, int, int);
template int Matrix::ArrayInverse<double>(double *, int, int);
template int Matrix::ArrayInverse<std::complex<float>>(std::complex<float> *, int, int);
template int Matrix::ArrayInverse<std::complex<double>>(std::complex<double> *, int, int);
template void Matrix::ArrayRowCanonize<std::complex<float> >(std::complex<float>*, int, int, std::complex<float>*);
template void Matrix::ArrayRowCanonize<std::complex<double> >(std::complex<double>*, int, int, std::complex<double>*);

}
