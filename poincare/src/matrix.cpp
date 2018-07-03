extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/global_context.h>
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include <poincare/addition.h>
#include <poincare/decimal.h>
#include <poincare/undefined.h>
#include <poincare/division.h>
#include <poincare/subtraction.h>
#include <poincare/multiplication.h>
#include "layout/matrix_layout.h"
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

Matrix::Matrix(MatrixData * matrixData) :
  DynamicHierarchy()
{
  assert(matrixData != nullptr);
  m_numberOfOperands = matrixData->numberOfRows()*matrixData->numberOfColumns();
  m_numberOfRows = matrixData->numberOfRows();
  matrixData->pilferOperands(&m_operands);
  for (int i = 0; i < m_numberOfOperands; i++) {
    const_cast<Expression *>(m_operands[i])->setParent(this);
  }
}

Matrix::Matrix(const Expression * const * operands, int numberOfRows, int numberOfColumns, bool cloneOperands) :
  DynamicHierarchy(operands, numberOfRows*numberOfColumns, cloneOperands),
  m_numberOfRows(numberOfRows)
{
}

int Matrix::numberOfRows() const {
  return m_numberOfRows;
}

int Matrix::numberOfColumns() const {
  return numberOfOperands()/m_numberOfRows;
}

Expression::Type Matrix::type() const {
  return Type::Matrix;
}

Expression * Matrix::clone() const {
  return new Matrix(m_operands, numberOfRows(), numberOfColumns(), true);
}

int Matrix::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
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
  for (int i = 0; i < numberOfRows(); i++) {
    buffer[currentChar++] = '[';
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    currentChar += operand(i*numberOfColumns())->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, numberOfSignificantDigits);
    if (currentChar >= bufferSize-1) {
      return currentChar;
    }
    for (int j = 1; j < numberOfColumns(); j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize-1) {
        return currentChar;
      }
      currentChar += operand(i*numberOfColumns()+j)->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, numberOfSignificantDigits);
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

int Matrix::polynomialDegree(char symbolName) const {
  return -1;
}

void Matrix::rowCanonize(Context & context, AngleUnit angleUnit, Multiplication * determinant) {
  // The matrix has to be reduced to be able to spot 0 inside it
  for (int i = 0; i < numberOfOperands(); i++) {
    editableOperand(i)->deepReduce(context, angleUnit);
  }
  int m = numberOfRows();
  int n = numberOfColumns();

  int h = 0; // row pivot
  int k = 0; // column pivot

  while (h < m && k < n) {
    // Find the first non-null pivot
    int iPivot = h;
    while (iPivot < m && matrixOperand(iPivot, k)->isRationalZero()) {
      iPivot++;
    }
    if (iPivot == m) {
      // No non-null coefficient in this column, skip
      k++;
      // Update determinant: det *= 0
      if (determinant) { determinant->addOperand(new Rational(0)); }
    } else {
      // Swap row h and iPivot
      if (iPivot != h) {
        for (int col = h; col < n; col++) {
          swapOperands(iPivot*n+col, h*n+col);
        }
        // Update determinant: det *= -1
        if (determinant) { determinant->addOperand(new Rational(-1)); }
      }
      /* Set to 1 M[h][k] by linear combination */
      Expression * divisor = matrixOperand(h, k);
      // Update determinant: det *= 1/divisor
      if (determinant) { determinant->addOperand(divisor->clone()); }
      for (int j = k+1; j < n; j++) {
        Expression * opHJ = matrixOperand(h, j);
        Expression * newOpHJ = new Division(opHJ, divisor->clone(), false);
        replaceOperand(opHJ, newOpHJ, false);
        newOpHJ->shallowReduce(context, angleUnit);
      }
      matrixOperand(h, k)->replaceWith(new Rational(1), true);

      /* Set to 0 all M[i][j] i != h, j > k by linear combination */
      for (int i = 0; i < m; i++) {
        if (i == h) { continue; }
        Expression * factor = matrixOperand(i, k);
        for (int j = k+1; j < n; j++) {
          Expression * opIJ = matrixOperand(i, j);
          Expression * newOpIJ = new Subtraction(opIJ, new Multiplication(matrixOperand(h, j), factor, true), false);
          replaceOperand(opIJ, newOpIJ, false);
          newOpIJ->editableOperand(1)->shallowReduce(context, angleUnit);
          newOpIJ->shallowReduce(context, angleUnit);
        }
        matrixOperand(i, k)->replaceWith(new Rational(0), true);
      }
      h++;
      k++;
    }
  }
}

LayoutRef Matrix::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  return CharLayoutRef('a'); //TODO
 /* assert(floatDisplayMode != PrintFloat::Mode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** childrenLayouts = new ExpressionLayout * [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * layout = new MatrixLayout(childrenLayouts, numberOfRows(), numberOfColumns(), false);
  delete [] childrenLayouts;
  return layout;*/
}

int Matrix::rank(Context & context, AngleUnit angleUnit, bool inPlace) {
  Matrix * m = inPlace ? this : static_cast<Matrix *>(clone());
  m->rowCanonize(context, angleUnit);
  int rank = m->numberOfRows();
  int i = rank-1;
  while (i >= 0) {
    int j = m->numberOfColumns()-1;
    while (j >= i && matrixOperand(i,j)->isRationalZero()) {
      j--;
    }
    if (j == i-1) {
      rank--;
    } else {
      break;
    }
    i--;
  }
  if (!inPlace) {
    delete m;
  }
  return rank;
}

template<typename T>
Complex<T> * Matrix::createTrace() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int dim = numberOfRows();
  Complex<T> c = Complex<T>::Float(0);
  for (int i = 0; i < dim; i++) {
    assert(operand(i*dim+i)->type() == Type::Complex);
    c = Addition::compute(c, *(static_cast<const Complex<T> *>(operand(i*dim+i))));
  }
  return new Complex<T>(c);
}

// TODO: 2. implement determinant/inverse for any expression (do not evaluate first)
template<typename T>
Complex<T> * Matrix::createDeterminant(Context & context, AngleUnit angleUnit) const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Matrix * copy = static_cast<Matrix *>(clone());
  Multiplication * m = new Multiplication();
  copy->rowCanonize(context, angleUnit, m);
  delete copy;
  Expression * determinant = m->approximate<T>(context, angleUnit);
  delete m;
  if (determinant->type() != Type::Complex) {
    delete determinant;
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  return static_cast<Complex<T> *>(determinant);
}

Expression * Matrix::createInverse(Context & context, AngleUnit angleUnit) const {
  if (numberOfRows() != numberOfColumns()) {
    return new Undefined();
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  const Expression ** operands = new const Expression * [dim*dim*2];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*2*dim+j] = operand(i*dim+j)->clone();
    }
    for (int j = dim; j < 2*dim; j++) {
      operands[i*2*dim+j] = j-dim == i ? new Rational(1) : new Rational(0);
    }
  }
  Matrix * AI = new Matrix(operands, dim, 2*dim, false);
  delete[] operands;
  AI->rowCanonize(context, angleUnit);
  // Check inversibility
  for (int i = 0; i < dim; i++) {
    if (AI->matrixOperand(i, i)->type() != Type::Rational || !static_cast<Rational *>(AI->matrixOperand(i, i))->isOne()) {
      delete AI;
      return new Undefined;
    }
  }
  const Expression ** invOperands = new const Expression * [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      invOperands[i*dim+j] = AI->matrixOperand(i, j+dim);
      AI->detachOperandAtIndex(i*2*dim+j+dim);
    }
  }
  Matrix * inverse = new Matrix(invOperands, dim, dim, false);
  delete[] invOperands;
  delete AI;
  return inverse;
}

template<typename T>
Matrix * Matrix::createApproximateInverse() const {
  for (int i = 0; i < numberOfOperands(); i++) {
    assert(operand(i)->type() == Type::Complex);
  }
  /* This method is called on already approximate matrices. The context and
   * angle unit parameters are therefore useless in createInverse and
   * approximate (as we work on Complex expressions only - no symbols or
   * trigonometric functions would required a context and angle unit). We feed
   * them with default parameters.*/
  GlobalContext context;
  Expression * inv = createInverse(context, AngleUnit::Degree);
  Expression * invApproximation = inv->approximate<T>(context, AngleUnit::Degree);
  delete inv;
  if (invApproximation->type() == Type::Matrix) {
    return static_cast<Matrix *>(invApproximation);
  }
  delete invApproximation;
  return nullptr;
}

Matrix * Matrix::createTranspose() const {
  const Expression ** operands = new const Expression * [numberOfOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = operand(i*numberOfColumns()+j);
    }
  }
  // Intentionally swapping dimensions for transpose
  Matrix * matrix = new Matrix(operands, numberOfColumns(), numberOfRows(), true);
  delete[] operands;
  return matrix;
}

Matrix * Matrix::createIdentity(int dim) {
  Expression ** operands = new Expression * [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = new Rational(1);
      } else {
        operands[i*dim+j] = new Rational(0);
      }
    }
  }
  Matrix * matrix = new Matrix(operands, dim, dim, false);
  delete [] operands;
  return matrix;
}

template<typename T>
Matrix * Matrix::createApproximateIdentity(int dim) {
  Expression ** operands = new Expression * [dim*dim];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      if (i == j) {
        operands[i*dim+j] = new Complex<T>(Complex<T>::Float(1));
      } else {
        operands[i*dim+j] = new Complex<T>(Complex<T>::Float(0));
      }
    }
  }
  Matrix * matrix = new Matrix(operands, dim, dim, false);
  delete [] operands;
  return matrix;
}

template<typename T>
Expression * Matrix::templatedApproximate(Context& context, AngleUnit angleUnit) const {
  Expression ** operands = new Expression * [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    Expression * operandEvaluation = operand(i)->approximate<T>(context, angleUnit);
    if (operandEvaluation->type() != Type::Complex) {
      operands[i] = new Complex<T>(Complex<T>::Float(NAN));
      delete operandEvaluation;
    } else {
      operands[i] = operandEvaluation;
    }
  }
  Expression * matrix = new Matrix(operands, numberOfRows(), numberOfColumns(), false);
  delete[] operands;
  return matrix;
}

template Complex<float>* Matrix::createTrace<float>() const;
template Complex<double>* Matrix::createTrace<double>() const;
template Complex<float>* Matrix::createDeterminant<float>(Context &, AngleUnit) const;
template Complex<double>* Matrix::createDeterminant<double>(Context &, AngleUnit) const;
template Matrix* Matrix::createApproximateInverse<float>() const;
template Matrix* Matrix::createApproximateInverse<double>() const;
template Matrix* Matrix::createApproximateIdentity<float>(int);
template Matrix* Matrix::createApproximateIdentity<double>(int);
}
