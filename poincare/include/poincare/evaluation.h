#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex>
extern "C" {
#include <stdint.h>
}
#include <poincare/expression.h>

namespace Poincare {

template<typename T>
class Evaluation {
public:
  enum class Type : uint8_t {
    Complex,
    MatrixComplex
  };
  virtual Type type() const = 0;
  virtual ~Evaluation() {}
  virtual bool isUndefined() const = 0;
  virtual T toScalar() const { return NAN; }
  virtual Expression * complexToExpression(Expression::ComplexFormat complexFormat) const = 0;
  virtual std::complex<T> createTrace() const = 0;
  virtual std::complex<T> createDeterminant() const = 0;
  virtual Evaluation * createInverse() const = 0;
  virtual Evaluation * createTranspose() const = 0;
};

template<typename T>
class Complex : public std::complex<T>, public Evaluation<T> {
public:
  Complex(T a, T b = 0.0) : std::complex<T>(a, b) {}
  Complex(std::complex<T> c) : std::complex<T>(c) {
    if (this->real() == -0) {
      this->real(0);
    }
    if (this->imag() == -0) {
      this->imag(0);
    }
  }
  static Complex Undefined() {
    return Complex(NAN, NAN);
  }
  virtual ~Complex() {}
  typename Poincare::Evaluation<T>::Type type() const override { return Poincare::Evaluation<T>::Type::Complex; }
  bool isUndefined() const override {
    return (std::isnan(this->real()) && std::isnan(this->imag()));
  }
  T toScalar() const override;
  Expression * complexToExpression(Expression::ComplexFormat complexFormat) const override;
  std::complex<T> createTrace() const override { return *this; }
  std::complex<T> createDeterminant() const override { return *this; }
  Complex<T> * createInverse() const override;
  Complex<T> * createTranspose() const override { return new Complex<T>(*this); }
};

template<typename T>
class MatrixComplex : public Evaluation<T> {
public:
  MatrixComplex(std::complex<T> * operands, int numberOfRows, int numberOfColumns);
  static MatrixComplex Undefined() {
    std::complex<T> undef = std::complex<T>(NAN, NAN);
    return MatrixComplex<T>(&undef, 1, 1);
  }
  virtual ~MatrixComplex() {}
  typename Poincare::Evaluation<T>::Type type() const override { return Poincare::Evaluation<T>::Type::MatrixComplex; }
  const std::complex<T> complexOperand(int i) const { return m_operands[i]; }
  int numberOfComplexOperands() const { return m_numberOfRows*m_numberOfColumns; }
  int numberOfRows() const { return m_numberOfRows; }
  int numberOfColumns() const { return m_numberOfColumns; }
  bool isUndefined() const override {
    return (numberOfRows() == 1 && numberOfColumns() == 1 && std::isnan(complexOperand(0).real()) && std::isnan(complexOperand(0).imag()));
  }
  Expression * complexToExpression(Expression::ComplexFormat complexFormat) const override;
  std::complex<T> createTrace() const override;
  std::complex<T> createDeterminant() const override;
  MatrixComplex<T> * createInverse() const override;
  MatrixComplex<T> * createTranspose() const override;
  static MatrixComplex<T> createIdentity(int dim);
private:
  std::complex<T> * m_operands;
  int m_numberOfRows;
  int m_numberOfColumns;
};

}

#endif
