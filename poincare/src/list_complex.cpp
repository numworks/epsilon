#include <poincare/list_complex.h>
#include <poincare/list.h>
#include <poincare/undefined.h>

namespace Poincare {

template<typename T>
std::complex<T> ListComplexNode<T>::complexAtIndex(int index) const {
  EvaluationNode<T> * child = EvaluationNode<T>::childAtIndex(index);
  if (child->type() == EvaluationNode<T>::Type::Complex) {
    return *(static_cast<ComplexNode<T> *>(child));
  }
  return std::complex<T>(NAN, NAN);
}

template<typename T>
Expression ListComplexNode<T>::complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const {
  List list = List::Builder();
  int i = 0;
  for (EvaluationNode<T> * c : this->children()) {
    Expression childExpression = Undefined::Builder();
    if (c->type() == EvaluationNode<T>::Type::Complex) {
      childExpression = c->complexToExpression(complexFormat);
    }
    if (childExpression.isUndefined()) {
      return Undefined::Builder();
    }
    list.addChildAtIndexInPlace(childExpression, i, i);
    i++;
  }
  return std::move(list);
}

template<typename T>
void ListComplex<T>::addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren) {
  if (t.type() != EvaluationNode<T>::Type::Complex) {
    t = Complex<T>::Undefined();
  }
  Evaluation<T>::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
}

template class ListComplexNode<float>;
template class ListComplexNode<double>;

template class ListComplex<float>;
template class ListComplex<double>;

}
