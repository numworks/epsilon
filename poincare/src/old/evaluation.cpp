#include <poincare/old/addition.h>
#include <poincare/old/complex.h>
#include <poincare/old/evaluation.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/old_expression.h>

namespace Poincare {

template <typename T>
bool EvaluationNode<T>::isDefinedScalar() const {
  return !std::isnan(toRealScalar());
}

template <typename T>
bool EvaluationNode<T>::isDefinedPoint() const {
  return otype() == EvaluationNode<T>::Type::PointEvaluation && !isUndefined();
}

template <typename T>
bool EvaluationNode<T>::isListOfDefinedScalars() const {
  if (otype() != EvaluationNode<T>::Type::ListComplex) {
    return false;
  }
  const int n = numberOfChildren();
  assert(n > 0);
  for (int i = 0; i < n; i++) {
    if (!childAtIndex(i)->isDefinedScalar()) {
      return false;
    }
  }
  return true;
}

template <typename T>
bool EvaluationNode<T>::isListOfDefinedPoints() const {
  if (otype() != EvaluationNode<T>::Type::ListComplex) {
    return false;
  }
  const int n = numberOfChildren();
  assert(n > 0);
  for (int i = 0; i < n; i++) {
    if (!childAtIndex(i)->isDefinedPoint()) {
      return false;
    }
  }
  return true;
}

template <typename T>
Evaluation<T> Evaluation<T>::childAtIndex(int i) const {
  PoolHandle c = PoolHandle::childAtIndex(i);
  return static_cast<Evaluation<T> &>(c);
}

template <typename T>
OExpression Evaluation<T>::complexToExpression(
    Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

template bool EvaluationNode<float>::isDefinedScalar() const;
template bool EvaluationNode<double>::isDefinedScalar() const;
template bool EvaluationNode<float>::isDefinedPoint() const;
template bool EvaluationNode<double>::isDefinedPoint() const;
template bool EvaluationNode<float>::isListOfDefinedScalars() const;
template bool EvaluationNode<double>::isListOfDefinedScalars() const;
template bool EvaluationNode<float>::isListOfDefinedPoints() const;
template bool EvaluationNode<double>::isListOfDefinedPoints() const;
template Evaluation<float> Evaluation<float>::childAtIndex(int i) const;
template Evaluation<double> Evaluation<double>::childAtIndex(int i) const;
template OExpression Evaluation<float>::complexToExpression(
    Preferences::ComplexFormat) const;
template OExpression Evaluation<double>::complexToExpression(
    Preferences::ComplexFormat) const;

}  // namespace Poincare
