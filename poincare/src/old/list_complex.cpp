#include <poincare/old/list.h>
#include <poincare/old/list_complex.h>
#include <poincare/old/list_sort.h>
#include <poincare/old/point_evaluation.h>
#include <poincare/old/undefined.h>

namespace Poincare {

template <typename T>
std::complex<T> ListComplexNode<T>::complexAtIndex(int index) const {
  assert(index < m_numberOfChildren);
  EvaluationNode<T> *child = EvaluationNode<T>::childAtIndex(index);
  if (child->otype() == EvaluationNode<T>::Type::Complex) {
    return *(static_cast<ComplexNode<T> *>(child));
  }
  return std::complex<T>(NAN, NAN);
}

template <typename T>
static bool isScalarEvaluationType(typename EvaluationNode<T>::Type type) {
  return type == EvaluationNode<T>::Type::Complex ||
         type == EvaluationNode<T>::Type::BooleanEvaluation ||
         type == EvaluationNode<T>::Type::PointEvaluation;
}

template <typename T>
OExpression ListComplexNode<T>::complexToExpression(
    Preferences::Preferences::ComplexFormat complexFormat) const {
  if (isUndefined()) {
    return Undefined::Builder();
  }
  OList list = OList::Builder();
  int i = 0;
  for (EvaluationNode<T> *c : this->children()) {
    OExpression childExpression = Undefined::Builder();
    if (isScalarEvaluationType<T>(c->otype())) {
      childExpression = c->complexToExpression(complexFormat);
    }
    list.addChildAtIndexInPlace(childExpression, i, i);
    i++;
  }
  return std::move(list);
}

template <typename T>
void ListComplex<T>::addChildAtIndexInPlace(Evaluation<T> t, int index,
                                            int currentNumberOfChildren) {
  assert(!node()->isUndefined());
  if (!isScalarEvaluationType<T>(t.otype())) {
    t = Complex<T>::Undefined();
  }
  Evaluation<T>::addChildAtIndexInPlace(t, index, currentNumberOfChildren);
}

template <typename T>
ListComplex<T> ListComplex<T>::Undefined() {
  ListComplex<T> undefList = ListComplex<T>::Builder();
  undefList.node()->setUndefined();
  return undefList;
}

template <typename T>
bool ListComplex<T>::sort() {
  if (numberOfChildren() == 0) {
    return true;
  }
  bool listOfDefinedScalars = this->isListOfDefinedScalars();
  bool listOfDefinedPoints = this->isListOfDefinedPoints();
  if (!listOfDefinedScalars && !listOfDefinedPoints) {
    return false;
  }
#if 0
  Helpers::ListSortPack<T> pack{nullptr, this, listOfDefinedScalars};
  Helpers::Sort(Helpers::SwapInList<T>, Helpers::CompareInList<T>, &pack,
                numberOfChildren());
#endif
  return true;
}

template class ListComplexNode<float>;
template class ListComplexNode<double>;

template class ListComplex<float>;
template class ListComplex<double>;

}  // namespace Poincare
