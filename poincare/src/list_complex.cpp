#include <poincare/list_complex.h>
#include <poincare/list.h>
#include <poincare/undefined.h>

namespace Poincare {

template<typename T>
std::complex<T> ListComplexNode<T>::complexAtIndex(int index) const {
  assert(index < m_numberOfChildren);
  EvaluationNode<T> * child = EvaluationNode<T>::childAtIndex(index);
  if (child->type() == EvaluationNode<T>::Type::Complex) {
    return *(static_cast<ComplexNode<T> *>(child));
  }
  return std::complex<T>(NAN, NAN);
}

template<typename T>
Expression ListComplexNode<T>::complexToExpression(Preferences::Preferences::ComplexFormat complexFormat) const {
  if (isUndefined()) {
    return Undefined::Builder();
  }
  List list = List::Builder();
  int i = 0;
  for (EvaluationNode<T> * c : this->children()) {
    Expression childExpression = Undefined::Builder();
    if (c->type() == EvaluationNode<T>::Type::Complex) {
      childExpression = c->complexToExpression(complexFormat);
    }
    list.addChildAtIndexInPlace(childExpression, i, i);
    i++;
  }
  return std::move(list);
}

template<typename T>
void ListComplex<T>::addChildAtIndexInPlace(Evaluation<T> t, int index, int currentNumberOfChildren) {
  assert(!node()->isUndefined());
  if (t.type() != EvaluationNode<T>::Type::Complex) {
    t = Complex<T>::Undefined();
  }
  Evaluation<T>::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
}

template<typename T>
ListComplex<T> ListComplex<T>::Undefined() {
  ListComplex<T> undefList = ListComplex<T>::Builder();
  undefList.node()->setUndefined();
  return undefList;
}

template class ListComplexNode<float>;
template class ListComplexNode<double>;

template class ListComplex<float>;
template class ListComplex<double>;

}
