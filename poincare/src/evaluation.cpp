#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/evaluation.h>
#include <poincare/expression.h>
#include <poincare/matrix_complex.h>
#include <poincare/multiplication.h>

namespace Poincare {

template <typename T>
Evaluation<T> Evaluation<T>::childAtIndex(int i) const {
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Evaluation<T> &>(c);
}

template <typename T>
Expression Evaluation<T>::complexToExpression(
    Preferences::ComplexFormat complexFormat) const {
  return node()->complexToExpression(complexFormat);
}

template Evaluation<float> Evaluation<float>::childAtIndex(int i) const;
template Evaluation<double> Evaluation<double>::childAtIndex(int i) const;
template Expression Evaluation<float>::complexToExpression(
    Preferences::ComplexFormat) const;
template Expression Evaluation<double>::complexToExpression(
    Preferences::ComplexFormat) const;

}  // namespace Poincare
