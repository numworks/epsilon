#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include <poincare/evaluation.h>

namespace Poincare {

template<typename T>
class Complex;

template<typename T>
class ComplexNode final : public std::complex<T>, public EvaluationNode<T> {
public:
  ComplexNode() : std::complex<T>(NAN, NAN) {}

  // TreeNode
  size_t size() const override { return sizeof(ComplexNode<T>); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Complex";
  }
  virtual void logAttributes(std::ostream & stream) const override {
    stream << " real=\"" << this->real() << "\"";
    stream << " imag=\"" << this->imag() << "\"";
  }
#endif

  virtual void setComplex(std::complex<T> c);
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
  explicit Complex(T a, T b = 0.0) : Complex(std::complex<T>(a, b)) {}
  explicit Complex(std::complex<T> c);
  static Complex<T> Undefined() {
    return Complex<T>(NAN, NAN);
  }
  std::complex<T> stdComplex() { return *node(); }
  T real() { return node()->real(); }
  T imag() { return node()->imag(); }
private:
  ComplexNode<T> * node() const { return static_cast<ComplexNode<T> *>(Evaluation<T>::node()); }
};


}

#endif
