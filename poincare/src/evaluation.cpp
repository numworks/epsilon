extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/evaluation.h>
#include <poincare/expression.h>
#include <poincare/undefined.h>
#include <poincare/decimal.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/subtraction.h>
#include <poincare/matrix.h>
#include <poincare/power.h>
#include <poincare/symbol.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

template<typename T>
T Complex<T>::toScalar() const {
  if (this->imag() == 0.0) {
    return this->real();
  }
  return NAN;
}

template <typename T>
static Expression * CreateDecimal(T f) {
  if (std::isnan(f) || std::isinf(f)) {
    return new Undefined();
  }
  return new Decimal(f);
}

template<typename T>
Expression * Complex<T>::complexToExpression(Expression::ComplexFormat complexFormat) const {
  if (std::isnan(this->real()) || std::isnan(this->imag()) || std::isinf(this->real()) || std::isinf(this->imag())) {
    return new Poincare::Undefined();
  }

  switch (complexFormat) {
    case Expression::ComplexFormat::Cartesian:
    {
      Expression * real = nullptr;
      Expression * imag = nullptr;
      if (this->real() != 0 || this->imag() == 0) {
        real = CreateDecimal(this->real());
      }
      if (this->imag() != 0) {
        if (this->imag() == 1.0 || this->imag() == -1) {
          imag = new Symbol(Ion::Charset::IComplex);
        } else if (this->imag() > 0) {
          imag = new Multiplication(CreateDecimal(this->imag()), new Symbol(Ion::Charset::IComplex), false);
        } else {
          imag = new Multiplication(CreateDecimal(-this->imag()), new Symbol(Ion::Charset::IComplex), false);
        }
      }
      if (imag == nullptr) {
        return real;
      } else if (real == nullptr) {
        if (this->imag() > 0) {
          return imag;
        } else {
          return new Opposite(imag, false);
        }
        return imag;
      } else if (this->imag() > 0) {
        return new Addition(real, imag, false);
      } else {
        return new Subtraction(real, imag, false);
      }
    }
    default:
    {
      assert(complexFormat == Expression::ComplexFormat::Polar);
      Expression * norm = nullptr;
      Expression * exp = nullptr;
      T r = std::abs(*this);
      T th = std::arg(*this);
      if (r != 1 || th == 0) {
        norm = CreateDecimal(r);
      }
      if (r != 0 && th != 0) {
        Expression * arg = nullptr;
        if (th == 1.0) {
          arg = new Symbol(Ion::Charset::IComplex);
        } else if (th == -1.0) {
          arg = new Opposite(new Symbol(Ion::Charset::IComplex), false);
        } else if (th > 0) {
          arg = new Multiplication(CreateDecimal(th), new Symbol(Ion::Charset::IComplex), false);
        } else {
          arg = new Opposite(new Multiplication(CreateDecimal(-th), new Symbol(Ion::Charset::IComplex), false));
        }
        exp = new Power(new Symbol(Ion::Charset::Exponential), arg, false);
      }
      if (exp == nullptr) {
        return norm;
      } else if (norm == nullptr) {
        return exp;
      } else {
        return new Multiplication(norm, exp, false);
      }
    }
  }
}

template<typename T>
std::complex<T> Complex<T>::pow(const std::complex<T> &c, const std::complex<T> &d) {
  std::complex<T> result = std::pow(c, d);
  /* Cheat: pow openbsd immplementationd is a numerical approximation.
   * We though want to avoid evaluating e^(i*pi) to -1+1E-17i. We thus round
   * real and imaginary parts to 0 if they are negligible compared to their
   * norm. */
  T norm = std::norm(result);
  if (norm !=  0 && std::fabs(result.real()/norm) <= Expression::epsilon<T>()) {
    result.real(0);
  }
  if (norm !=  0 && std::fabs(result.imag()/norm) <= Expression::epsilon<T>()) {
    result.imag(0);
  }
  if (c.imag() == 0 && d.imag() == 0 && (c.real() >= 0 || d.real() == std::round(d.real()))) {
    result.imag(0);
  }
  return result;
}

template<typename T>
std::complex<T> Complex<T>::approximate(const std::complex<T>& c, ComplexFunction<T> function) {
  std::complex<T> result = function(c);
  /* Cheat: openbsd trigonometric functions (cos, sin, tan, sqrt) are numerical
   * implementation and thus are approximative. The error epsilon is ~1E-7
   * on float and ~1E-15 on double. In order to avoid weird results as
   * cos(90) = 6E-17, we neglect the result when its ratio with the argument
   * (pi in the exemple) is smaller than epsilon.
   * We can't do that for all evaluation as the user can operate on values as
   * small as 1E-308 (in double) and most results still be correct. */
  T inputNorm = std::abs(c);
  if (inputNorm !=  0 && std::fabs(result.real()/inputNorm) <= Expression::epsilon<T>()) {
    result.real(0);
  }
  if (inputNorm !=  0 && std::fabs(result.imag()/inputNorm) <= Expression::epsilon<T>()) {
    result.imag(0);
  }
  return result;
}

template<typename T>
MatrixComplex<T>::MatrixComplex(std::complex<T> * operands, int numberOfRows, int numberOfColumns) :
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  m_operands = new std::complex<T> [numberOfRows*numberOfColumns];
  for (int i=0; i<numberOfRows*numberOfColumns; i++) {
    m_operands[i] = operands[i];
    if (m_operands[i].real() == -0.0) {
      m_operands[i].real(0.0);
    }
    if (m_operands[i].imag() == -0.0) {
      m_operands[i].imag(0.0);
    }
  }
}

template<typename T>
Expression * MatrixComplex<T>::complexToExpression(Expression::ComplexFormat complexFormat) const {
  Expression ** operands = new Expression * [numberOfComplexOperands()];
  for (int i = 0; i < numberOfComplexOperands(); i++) {
    operands[i] = Complex<T>(complexOperand(i)).complexToExpression(complexFormat);
  }
  Expression * result = new Matrix(operands, numberOfRows(), numberOfColumns(), false);
  delete[] operands;
  return result;
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

// TODO: implement determinant/inverse for any expression (do not evaluate first)
template<typename T>
std::complex<T> MatrixComplex<T>::createDeterminant() const {
  if (numberOfRows() != numberOfColumns()) {
    return std::complex<T>(NAN, NAN);
  }
  int dim = numberOfRows();
  std::complex<T> ** tempMat = new std::complex<T>*[dim];
  for (int i = 0; i < dim; i++) {
    tempMat[i] = new std::complex<T>[dim];
  }
  std::complex<T> det = 1;
  /* Copy the matrix */
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      tempMat[i][j] = complexOperand(i*dim+j);
    }
  }

  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim-1; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (std::abs(tempMat[rowWithPivot][i]) < std::abs(tempMat[row][i])) {
        rowWithPivot = row;
      }
    }
    std::complex<T> valuePivot = tempMat[rowWithPivot][i];
    /* if the pivot is null, det = 0. */
    if (std::abs(valuePivot) <= (sizeof(T) == sizeof(float) ? FLT_EPSILON : DBL_EPSILON)) {
      det = 0;
      break;
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < dim; col++) {
        std::complex<T> temp = tempMat[i][col];
        tempMat[i][col] = tempMat[rowWithPivot][col];
        tempMat[rowWithPivot][col] = temp;
      }
      det *= -1;
    }
    det *= valuePivot;
    /* Set to 0 all A[][i] by linear combination */
    for (int row = i+1; row < dim; row++) {
      std::complex<T> factor = tempMat[row][i]/valuePivot;
      for (int col = i; col < dim; col++) {
        tempMat[row][col] -= factor*tempMat[i][col];
      }
    }
  }
  det *= tempMat[dim-1][dim-1];
  for (int i = 0; i < dim; i++) {
    delete[] tempMat[i];
  }
  delete[] tempMat;
  return det;
}

template<typename T>
MatrixComplex<T> MatrixComplex<T>::createInverse() const {
  if (numberOfRows() != numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  std::complex<T> ** inv = new std::complex<T>*[dim];
  for (int i = 0; i < dim; i++) {
    inv[i] = new std::complex<T> [2*dim];
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      inv[i][j] =  complexOperand(i*dim+j);
    }
    for (int j = dim; j < 2*dim; j++) {
      inv[i][j] = (i+dim == j);
    }
  }
  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (std::abs(inv[rowWithPivot][i]) < std::abs(inv[row][i])) {
        rowWithPivot = row;
      }
    }
    std::complex<T> valuePivot = inv[rowWithPivot][i];
    /* if the pivot is null, the matrix in not invertible. */
    if (std::abs(valuePivot) <= (sizeof(T) == sizeof(float) ? FLT_EPSILON : DBL_EPSILON)) {
      for (int i = 0; i < dim; i++) {
        delete[] inv[i];
      }
      delete[] inv;
      return MatrixComplex<T>::Undefined();
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < 2*dim; col++) {
        std::complex<T> temp = inv[i][col];
        inv[i][col] = inv[rowWithPivot][col];
        inv[rowWithPivot][col] = temp;
      }
    }
    /* A[pivot][] = A[pivot][]/valuePivot */
    for (int col = 0; col < 2*dim; col++) {
      inv[i][col] /= valuePivot;
    }
    /* Set to 0 all A[][row] by linear combination */
    for (int row = 0; row < dim; row++) {
      if (row == i) {
        continue;
      }
      std::complex<T> factor = inv[row][i];
      for (int col = 0; col < 2*dim; col++) {
        inv[row][col] -= factor*inv[i][col];
      }
    }
  }
  std::complex<T> * operands = new std::complex<T> [numberOfComplexOperands()];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*dim+j] = inv[i][j+dim];
    }
  }
  for (int i = 0; i < dim; i++) {
    delete[] inv[i];
  }
  delete[] inv;
  // Intentionally swapping dimensions for inverse, although it doesn't make a difference because it is square
  MatrixComplex<T> matrix = MatrixComplex<T>(operands, numberOfColumns(), numberOfRows());
  delete[] operands;
  return matrix;
}

template<typename T>
MatrixComplex<T> MatrixComplex<T>::createTranspose() const {
  std::complex<T> * operands = new std::complex<T> [numberOfComplexOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = complexOperand(i*numberOfColumns()+j);
    }
  }
  // Intentionally swapping dimensions for transpose
  MatrixComplex<T> matrix = MatrixComplex<T>(operands, numberOfColumns(), numberOfRows());
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
