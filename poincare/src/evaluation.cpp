extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/evaluation.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
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
          arg = new Opposite(new Multiplication(CreateDecimal(-th), new Symbol(Ion::Charset::IComplex), false), false);
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
    if (m_operands[i].real() == -0.0) {
      m_operands[i].real(0.0);
    }
    if (m_operands[i].imag() == -0.0) {
      m_operands[i].imag(0.0);
    }
  }
}

template<typename T>
MatrixComplex<T>::~MatrixComplex() {
  if (m_operands != nullptr) {
    delete [] m_operands;
  }
}

template<typename T>
MatrixComplex<T>::MatrixComplex(MatrixComplex&& other) {
  // Pilfer other's data
  m_operands = other.m_operands;
  m_numberOfRows = other.m_numberOfRows;
  m_numberOfColumns = other.m_numberOfColumns;

  // Reset other
  other.m_operands = nullptr;
  other.m_numberOfRows = 0;
  other.m_numberOfColumns = 0;
}

template<typename T>
MatrixComplex<T>::MatrixComplex(const MatrixComplex& other) {
  // Copy other's data
  m_numberOfRows = other.m_numberOfRows;
  m_numberOfColumns = other.m_numberOfColumns;
  std::complex<T> * operands = new std::complex<T> [m_numberOfRows*m_numberOfColumns];
  for (int i=0; i<m_numberOfRows*m_numberOfColumns; i++) {
    operands[i] = other.m_operands[i];
  }
  m_operands = operands;
}

template<typename T>
MatrixComplex<T>& MatrixComplex<T>::operator=(MatrixComplex<T> && other) {
  if (this != &other) {
    if (m_operands) { delete [] m_operands; }
    // Pilfer other's ivars
    m_operands = other.m_operands;
    m_numberOfRows = other.m_numberOfRows;
    m_numberOfColumns = other.m_numberOfColumns;

    // Reset other
    other.m_operands = nullptr;
    other.m_numberOfRows = 0;
    other.m_numberOfColumns = 0;
  }
  return *this;
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
