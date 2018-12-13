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
  T ra, tb;
  if (complexFormat == Preferences::ComplexFormat::Cartesian) {
    ra = this->real();
    tb = this->imag();
  } else {
    ra = std::abs(*this);
    tb = std::arg(*this);
  }
  return Expression::CreateComplexExpression(
      Number::DecimalNumber<T>(std::fabs(ra)),
      Number::DecimalNumber<T>(std::fabs(tb)),
      complexFormat,
      (std::isnan(this->real()) || std::isnan(this->imag())),
      ra == 0.0, ra == 1.0, tb == 0.0, tb == 1.0, tb == -1.0, ra < 0.0, tb < 0.0
    );
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
