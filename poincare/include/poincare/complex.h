#ifndef POINCARE_COMPLEX_H
#define POINCARE_COMPLEX_H

#include <poincare/evaluation.h>

namespace Poincare {

template<typename T>
class ComplexReference;

template<typename T>
class ComplexNode : public std::complex<T>, public EvaluationNode<T> {
public:
  // TreeNode
  size_t size() const override { return sizeof(ComplexNode<T>); }
#if TREE_LOG
  const char * description() const override { return "Evaluation complex";  }
#endif
  int numberOfChildren() const override { return 0; }

  void setComplex(std::complex<T> c);
  typename Poincare::EvaluationNode<T>::Type type() const override { return Poincare::EvaluationNode<T>::Type::Complex; }
  bool isUndefined() const override {
    return (std::isnan(this->real()) && std::isnan(this->imag()));
  }
  T toScalar() const override;
  ExpressionReference complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const override;
  std::complex<T> trace() const override { return *this; }
  std::complex<T> determinant() const override { return *this; }
  EvaluationReference<T> inverse() const override;
  EvaluationReference<T> transpose() const override { return ComplexReference<T>(*this); }
};

template<typename T>
class ComplexReference : public std::complex<T>, public EvaluationReference<T> {
public:
  ComplexReference(TreeNode * t) : EvaluationReference<T>(t) {}
  ComplexReference(T a, T b = 0.0) : ComplexReference(std::complex<T>(a, b)) {}
  ComplexReference(std::complex<T> c);
  static ComplexReference<T> Undefined() {
    return ComplexReference<T>(NAN, NAN);
  }
};


}

#endif
