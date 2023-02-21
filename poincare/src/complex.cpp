extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <ion.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/constant.h>
#include <poincare/decimal.h>
#include <poincare/division.h>
#include <poincare/expression.h>
#include <poincare/infinity.h>
#include <poincare/multiplication.h>
#include <poincare/nonreal.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

#include <cmath>

namespace Poincare {

template <typename T>
ComplexNode<T>::ComplexNode(std::complex<T> c)
    : EvaluationNode<T>(), std::complex<T>(c.real(), c.imag()) {
  if (!std::isnan(c.imag()) && c.imag() != static_cast<T>(0.0)) {
    Expression::SetEncounteredComplex(true);
  }
  if (this->real() == -0) {
    this->real(0);
  }
  if (this->imag() == -0) {
    this->imag(0);
  }
}

template <typename T>
T ComplexNode<T>::ToScalar(const std::complex<T> c) {
  return c.imag() == static_cast<T>(0.0) ? c.real() : NAN;
}

template <typename T>
T ComplexNode<T>::toScalar() const {
  return ToScalar(*this);
}

template <typename T>
Expression ComplexNode<T>::complexToExpression(
    Preferences::ComplexFormat complexFormat) const {
  if (complexFormat == Preferences::ComplexFormat::Real &&
      Expression::EncounteredComplex()) {
    return Nonreal::Builder();
  }
  T ra, tb;
  if (complexFormat == Preferences::ComplexFormat::Polar) {
    ra = std::abs(*this);
    tb = std::arg(*this);
  } else {
    ra = this->real();
    tb = this->imag();
  }
  return Expression::CreateComplexExpression(
      Number::FloatNumber<T>(std::fabs(ra)),
      Number::FloatNumber<T>(std::fabs(tb)), complexFormat,
      (std::isnan(this->real()) || std::isnan(this->imag())),
      ra == static_cast<T>(0.0), std::fabs(ra) == static_cast<T>(1.0),
      tb == static_cast<T>(0.0), std::fabs(tb) == static_cast<T>(1.0),
      ra < static_cast<T>(0.0), tb < static_cast<T>(0.0));
}

template <typename T>
Complex<T> Complex<T>::Builder(std::complex<T> c) {
  void *bufferNode = TreePool::sharedPool->alloc(sizeof(ComplexNode<T>));
  ComplexNode<T> *node = new (bufferNode) ComplexNode<T>(c);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<Complex<T> &>(h);
}

template class ComplexNode<float>;
template class ComplexNode<double>;
template Complex<float> Complex<float>::Builder(float a, float b);
template Complex<double> Complex<double>::Builder(double a, double b);
template Complex<float> Complex<float>::Builder(std::complex<float> c);
template Complex<double> Complex<double>::Builder(std::complex<double> c);

}  // namespace Poincare
