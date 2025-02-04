#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include "evaluation.h"

namespace Poincare {

template <typename T>
class Complex;

template <typename T>
class ComplexNode final : public EvaluationNode<T>, public std::complex<T> {
 public:
  static T ToRealScalar(const std::complex<T> c);
  ComplexNode(std::complex<T> c);

  std::complex<T> complexAtIndex(int index) const override {
    assert(index == 0);
    return *this;
  }

  // PoolObject
  size_t size() const override { return sizeof(ComplexNode<T>); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream &stream) const override { stream << "Complex"; }
  void logAttributes(std::ostream &stream) const override {
    stream << " real=\"" << this->real() << "\"";
    stream << " imag=\"" << this->imag() << "\"";
  }
#endif

  typename Poincare::EvaluationNode<T>::Type otype() const override {
    return Poincare::EvaluationNode<T>::Type::Complex;
  }
  bool isUndefined() const override {
    return (std::isnan(this->real()) || std::isnan(this->imag()));
  }
  T toRealScalar() const override;
  OExpression complexToExpression(
      Preferences::Preferences::ComplexFormat complexFormat) const override;
};

template <typename T>
std::complex<T> complexNAN() {
  return std::complex<T>(NAN, NAN);
}

template <typename T>
std::complex<T> complexRealNAN() {
  return std::complex<T>(NAN, 0.0);
}

template <typename T>
class Complex final : public Evaluation<T> {
 public:
  Complex(ComplexNode<T> *n) : Evaluation<T>(n) {}
  static Complex<T> Builder(std::complex<T> c);
  static Complex<T> Builder(T a, T b = 0.0) {
    return Complex<T>::Builder(std::complex<T>(a, b));
  }
  static Complex<T> Undefined() { return Complex<T>::Builder(complexNAN<T>()); }
  static Complex<T> RealUndefined() {
    return Complex<T>::Builder(complexRealNAN<T>());
  }
  T real() { return node()->real(); }
  T imag() { return node()->imag(); }

 private:
  ComplexNode<T> *node() const {
    return static_cast<ComplexNode<T> *>(Evaluation<T>::node());
  }
};

}  // namespace Poincare

#endif
