#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include <poincare/evaluation.h>

namespace Poincare {

template<typename T>
class Complex;

template<typename T>
class ComplexNode final : public EvaluationNode<T>, public std::complex<T> {
public:
  ComplexNode(std::complex<T> c);

  // TreeNode
  size_t size() const override { return sizeof(ComplexNode<T>); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Complex";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " real=\"" << this->real() << "\"";
    stream << " imag=\"" << this->imag() << "\"";
  }
#endif

  typename Poincare::EvaluationNode<T>::Type type() const override { return Poincare::EvaluationNode<T>::Type::Complex; }
  bool isUndefined() const override {
    return (std::isnan(this->real()) && std::isnan(this->imag()));
  }
  T toScalar() const override;
  Expression complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override { return *this; }
  std::complex<T> determinant() const override { return *this; }
};

template<typename T>
class Complex final : public Evaluation<T> {
public:
  Complex(ComplexNode<T> * n) : Evaluation<T>(n) {}
  static Complex<T> Builder(std::complex<T> c);
  static Complex<T> Builder(T a, T b = 0.0) { return Complex<T>::Builder(std::complex<T>(a, b)); }
  static Complex<T> Undefined() {
    return Complex<T>::Builder(NAN, NAN);
  }
  static Complex<T> RealUndefined() {
    return Complex<T>::Builder(NAN, 0.0);
  }
  std::complex<T> stdComplex() { return *node(); }
  T real() { return node()->real(); }
  T imag() { return node()->imag(); }
private:
  ComplexNode<T> * node() const { return static_cast<ComplexNode<T> *>(Evaluation<T>::node()); }
};


}

#endif
