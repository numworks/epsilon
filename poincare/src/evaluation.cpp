#include <poincare/evaluation.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/expression.h>
#include <poincare/matrix_complex.h>

namespace Poincare {

template<typename T>
Evaluation<T> Evaluation<T>::childAtIndex(int i) const {
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Evaluation<T> &>(c);
}

template<typename T>
Expression Evaluation<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

template<typename T>
Evaluation<T> Evaluation<T>::Sum(Evaluation<T> a, Evaluation<T> b, Preferences::ComplexFormat complexFormat) {
  assert(a.type() != EvaluationNode<T>::Type::Exception && b.type() != EvaluationNode<T>::Type::Exception);
  if (a.type() == EvaluationNode<T>::Type::Complex) {
    if (b.type() == EvaluationNode<T>::Type::Complex) {
      return AdditionNode::compute(static_cast<Complex<T> &>(a).stdComplex(), static_cast<Complex<T> &>(b).stdComplex(), complexFormat);
    }
    return AdditionNode::computeOnComplexAndMatrix(static_cast<Complex<T> &>(a).stdComplex(), static_cast<MatrixComplex<T> &>(b), complexFormat);
  }
  if (b.type() == EvaluationNode<T>::Type::Complex) {
    return AdditionNode::computeOnComplexAndMatrix(static_cast<Complex<T> &>(b).stdComplex(), static_cast<MatrixComplex<T> &>(a), complexFormat);
  }
  return AdditionNode::computeOnMatrices(static_cast<MatrixComplex<T> &>(a), static_cast<MatrixComplex<T> &>(b), complexFormat);
}

template Evaluation<float> Evaluation<float>::childAtIndex(int i) const;
template Evaluation<double> Evaluation<double>::childAtIndex(int i) const;
template Expression Evaluation<float>::complexToExpression(Preferences::ComplexFormat) const;
template Expression Evaluation<double>::complexToExpression(Preferences::ComplexFormat) const;
template Evaluation<float> Evaluation<float>::Sum(Evaluation<float> a, Evaluation<float> b, Preferences::ComplexFormat complexFormat);
template Evaluation<double> Evaluation<double>::Sum(Evaluation<double> a, Evaluation<double> b, Preferences::ComplexFormat complexFormat);

}
