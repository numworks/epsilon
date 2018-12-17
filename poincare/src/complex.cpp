extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/complex.h>
#include <poincare/division.h>
#include <poincare/expression.h>
#include <poincare/undefined.h>
#include <poincare/infinity.h>
#include <poincare/decimal.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/constant.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

template<typename T>
void ComplexNode<T>::setComplex(std::complex<T> c) {
  this->real(c.real());
  this->imag(c.imag());
  if (this->real() == -0) {
    this->real(0);
  }
  if (this->imag() == -0) {
    this->imag(0);
  }
}

template<typename T>
T ComplexNode<T>::toScalar() const {
  if (this->imag() == 0.0) {
    return this->real();
  }
  return NAN;
}

template<typename T>
Expression ComplexNode<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  if (std::isnan(this->real()) || std::isnan(this->imag())) {
    return Undefined();
  }
  if (complexFormat == Preferences::ComplexFormat::Cartesian) {
      Expression real;
      Expression imag;
      if (this->real() != 0 || this->imag() == 0) {
        real = Number::DecimalNumber<T>(this->real());
      }
      if (this->imag() != 0) {
        if (this->imag() == 1.0 || this->imag() == -1) {
          imag = Constant(Ion::Charset::IComplex);
        } else if (this->imag() > 0) {
          imag = Multiplication(Number::DecimalNumber(this->imag()), Constant(Ion::Charset::IComplex));
        } else {
          imag = Multiplication(Number::DecimalNumber(-this->imag()), Constant(Ion::Charset::IComplex));
        }
      }
      if (imag.isUninitialized()) {
        return real;
      } else if (real.isUninitialized()) {
        if (this->imag() > 0) {
          return imag;
        } else {
          return Opposite(imag);
        }
        return imag;
      } else if (this->imag() > 0) {
        return Addition(real, imag);
      } else {
        return Subtraction(real, imag);
      }
  }
  assert(complexFormat == Preferences::ComplexFormat::Polar);
  Expression norm;
  Expression exp;
  T r = std::abs(*this);
  T th = std::arg(*this);
  if (r != 1 || th == 0) {
    norm = Number::DecimalNumber(r);
  }
  if (r != 0 && th != 0) {
    Expression arg;
    if (th == 1.0) {
      arg = Constant(Ion::Charset::IComplex);
    } else if (th == -1.0) {
      arg = Opposite(Constant(Ion::Charset::IComplex));
    } else if (th > 0) {
      arg = Multiplication(Number::DecimalNumber(th), Constant(Ion::Charset::IComplex));
    } else {
      arg = Opposite(Multiplication(Number::DecimalNumber(-th), Constant(Ion::Charset::IComplex)));
    }
    exp = Power(Constant(Ion::Charset::Exponential), arg);
  }
  if (exp.isUninitialized()) {
    return norm;
  } else if (norm.isUninitialized()) {
    return exp;
  } else {
    return Multiplication(norm, exp);
  }
}

template <typename T>
Complex<T>::Complex(std::complex<T> c) :
  Evaluation<T>(TreePool::sharedPool()->createTreeNode<ComplexNode<T>>())
{
  node()->setComplex(c);
}

template class ComplexNode<float>;
template class ComplexNode<double>;
template Complex<float>::Complex(float a, float b);
template Complex<double>::Complex(double a, double b);
template Complex<float>::Complex(std::complex<float> c);
template Complex<double>::Complex(std::complex<double> c);

}
