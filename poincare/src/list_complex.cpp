#include <poincare/list.h>
#include <poincare/list_complex.h>
#include <poincare/list_sort.h>
#include <poincare/point_evaluation.h>
#include <poincare/undefined.h>

namespace Poincare {

template <typename T>
std::complex<T> ListComplexNode<T>::complexAtIndex(int index) const {
  assert(index < m_numberOfChildren);
  EvaluationNode<T> *child = EvaluationNode<T>::childAtIndex(index);
  if (child->type() == EvaluationNode<T>::Type::Complex) {
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
Expression ListComplexNode<T>::complexToExpression(
    Preferences::Preferences::ComplexFormat complexFormat) const {
  if (isUndefined()) {
    return Undefined::Builder();
  }
  List list = List::Builder();
  int i = 0;
  for (EvaluationNode<T> *c : this->children()) {
    Expression childExpression = Undefined::Builder();
    if (isScalarEvaluationType<T>(c->type())) {
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
  if (!isScalarEvaluationType<T>(t.type())) {
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
ListComplex<T> ListComplex<T>::sort() {
  void *ctx = this;
  Helpers::Sort(
      // Swap
      [](int i, int j, void *context, int n) {
        ListComplex<T> *list = reinterpret_cast<ListComplex<T> *>(context);
        assert(list->numberOfChildren() == n && 0 <= i && 0 <= j && i < n &&
               j < n);
        list->swapChildrenInPlace(i, j);
      },
      // Compare
      [](int i, int j, void *context, int numberOfElements) {
        ListComplex<T> *list = reinterpret_cast<ListComplex<T> *>(context);

        Evaluation<T> eI = list->childAtIndex(i);
        Evaluation<T> eJ = list->childAtIndex(j);
        if (eI.type() == EvaluationNode<T>::Type::PointEvaluation) {
          if (eJ.isUndefined()) {
            return !ListSort::k_nanIsGreatest;
          } else if (eJ.type() == EvaluationNode<T>::Type::PointEvaluation) {
            return static_cast<PointEvaluation<T> &>(eI).xy().isGreaterThan(
                static_cast<PointEvaluation<T> &>(eJ).xy(),
                ListSort::k_nanIsGreatest);
          }
        }

        float xI = eI.toScalar();
        float xJ = eJ.toScalar();
        return Helpers::FloatIsGreater(xI, xJ, ListSort::k_nanIsGreatest);
      },
      ctx, numberOfChildren());
  return *this;
}

template class ListComplexNode<float>;
template class ListComplexNode<double>;

template class ListComplex<float>;
template class ListComplex<double>;

}  // namespace Poincare
