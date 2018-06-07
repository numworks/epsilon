#ifndef POINCARE_EVALUATION_H
#define POINCARE_EVALUATION_H

#include <complex>
extern "C" {
#include <stdint.h>
}
#include <poincare/expression.h>

namespace Poincare {

/* acos, asin, arctan, acosh, asinh, arctanh and log have branch cuts on the
 * complex plan.
 * - acos, asin, atanh: ]-inf, -1[U]1, +inf[
 * - atan, asinh: ]-inf*i, -i[U]i, +inf*i[
 * - acosh: ]-inf, 1]
 * - log: ]-inf, 0]
 * They are then multivalued on these cuts. We followed the convention chosen
 * by the lib c++ of llvm but it might differ from a calculator to another
 * (ie acos(2) = -1.3169578969248*I or 1.3169578969248*I). */

template<typename T>
class Evaluation {
public:
  enum class Type : uint8_t {
    Complex,
    MatrixComplex
  };
  virtual Type type() const = 0;
  virtual ~Evaluation() {}
  virtual T toScalar() const { return NAN; }
  virtual Expression * complexToExpression(Expression::ComplexFormat complexFormat) const = 0;
  virtual std::complex<T> createTrace() const = 0;
  virtual std::complex<T> createDeterminant() const = 0;
};

template <typename T>
using ComplexFunction = std::complex<T> (*)(const std::complex<T>&);

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
  T toScalar() const override;
  Expression * complexToExpression(Expression::ComplexFormat complexFormat) const override;
  std::complex<T> createTrace() const override { return *this; }
  std::complex<T> createDeterminant() const override { return *this; }
  /* Complex functions */
  static std::complex<T> pow(const std::complex<T> &c, const std::complex<T> &d);
  static std::complex<T> sqrt(const std::complex<T> &c) {
    return approximate(c, std::sqrt);
  }
  static std::complex<T> cos(const std::complex<T> &c) {
    return approximate(c, std::cos);
  }
  static std::complex<T> sin(const std::complex<T> &c) {
    return approximate(c, std::sin);
  }
  static std::complex<T> tan(const std::complex<T> &c) {
    return approximate(c, std::tan);
  }
  static std::complex<T> approximate(const std::complex<T>& c, ComplexFunction<T> approximation);
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
  Expression * complexToExpression(Expression::ComplexFormat complexFormat) const override;
  std::complex<T> createTrace() const override;
  std::complex<T> createDeterminant() const override;
  MatrixComplex<T> createInverse() const;
  MatrixComplex<T> createTranspose() const;
  static MatrixComplex<T> createIdentity(int dim);
private:
  std::complex<T> * m_operands;
  int m_numberOfRows;
  int m_numberOfColumns;
};

}

#endif
