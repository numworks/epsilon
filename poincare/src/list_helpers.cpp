#include <poincare/list_helpers.h>
#include <poincare/addition.h>
#include <poincare/approximation_helper.h>
#include <poincare/helpers.h>
#include <poincare/expression_node.h>
#include <poincare/list_sort.h>
#include <poincare/multiplication.h>
#include <poincare/undefined.h>

namespace Poincare {

int ListHelpers::LengthOfListChildren(const Expression e) {
  int lastLength = k_noList;
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (e.childAtIndex(i).type() == ExpressionNode::Type::List) {
      int length = e.childAtIndex(i).numberOfChildren();
      if (lastLength == k_noList) {
        lastLength = length;
      } else if (lastLength != length) {
        return k_mismatchedLists;
      }
    }
  }
  return lastLength;
}

template <typename T> Evaluation<T> ListHelpers::ExtremumApproximationOfListNode(ListNode * listNode, ExpressionNode::ApproximationContext approximationContext, bool minimum) {
    if (listNode->numberOfChildren() == 0) {
      return Complex<T>::Undefined();
    }
    return listNode->childAtIndex(ExtremumOfListNodeIndex(listNode, approximationContext, minimum))->approximate(static_cast<T>(0), approximationContext);
}

Expression ListHelpers::ExtremumOfList(List list, ExpressionNode::ReductionContext reductionContext, bool minimum) {
  if (list.numberOfChildren() == 0) {
    return Undefined::Builder();
  }
  ExpressionNode::ApproximationContext approximationContext(reductionContext, true);
  return list.childAtIndex(ExtremumOfListNodeIndex(list.node(), approximationContext, minimum));
}

bool ListHelpers::ListEvaluationComparison(int i, int j, void * context, int numberOfElements) {
  void ** c = reinterpret_cast<void **>(context);
  ListNode * list = reinterpret_cast<ListNode *>(c[0]);
  ExpressionNode::ApproximationContext * approximationContext = reinterpret_cast<ExpressionNode::ApproximationContext *>(c[1]);
  bool * nanIsGreatest = reinterpret_cast<bool *>(c[2]);
  float xI = list->childAtIndex(i)->approximate(static_cast<float>(0), *approximationContext).toScalar();
  float xJ =  list->childAtIndex(j)->approximate(static_cast<float>(0), *approximationContext).toScalar();
  return Poincare::Helpers::FloatIsGreater(xI, xJ, *nanIsGreatest);
}

int ListHelpers::ExtremumOfListNodeIndex(ListNode * listNode, ExpressionNode::ApproximationContext approximationContext, bool minimum) {
  void * comparisonContext[] = {listNode, &approximationContext, &minimum};
  return Poincare::Helpers::ExtremumIndex(
      ListEvaluationComparison,
      comparisonContext,
      listNode->numberOfChildren(),
      minimum);
}

template<typename T> Evaluation<T> ListHelpers::SumOfListNode(ListNode * list, ExpressionNode::ApproximationContext approximationContext) {
  return ApproximationHelper::MapReduce<T>(
      list,
      approximationContext,
      [] (Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
      return ApproximationHelper::Reduce<T>(
          eval1,
          eval2,
          complexFormat,
          AdditionNode::computeOnComplex<T>,
          ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
          ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
          ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
          );
      }
      );
}

template<typename T> Evaluation<T> ListHelpers::ProductOfListNode(ListNode * list, ExpressionNode::ApproximationContext approximationContext) {
  return ApproximationHelper::MapReduce<T>(
      list,
      approximationContext,
      [] (Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
      return ApproximationHelper::Reduce<T>(
          eval1,
          eval2,
          complexFormat,
          MultiplicationNode::computeOnComplex<T>,
          ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
          ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
          ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
          );
      }
      );
}

template<typename T> Evaluation<T> ListHelpers::SquareSumOfListNode(ListNode * list, ExpressionNode::ApproximationContext approximationContext) {
  return ApproximationHelper::MapReduce<T>(
      list,
      approximationContext,
      [] (Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
      return ApproximationHelper::Reduce<T>(
          eval1,
          eval2,
          complexFormat,
          [] (const std::complex<T> c1, const std::complex<T> c2, Preferences::ComplexFormat complexFormat) {
            return AdditionNode::computeOnComplex(c1, MultiplicationNode::computeOnComplex<T>(c2, c2, complexFormat).complexAtIndex(0), complexFormat);
          },
          ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
          ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
          ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
          );
      }
      );
 }

template<typename T> ListComplex<T> ListHelpers::SortListComplex(ListComplex<T> list) {
  void * ctx = &list;
  Helpers::Sort(
      // Swap
      [](int i, int j, void * context, int n) {
        ListComplex<T> * list = reinterpret_cast<ListComplex<T> *>(context);
        assert(list->numberOfChildren() == n && 0 <= i && 0 <= j && i < n && j < n);
        list->swapChildrenInPlace(i, j);
      },
      // Compare
      [](int i, int j, void * context, int numberOfElements) {
      ListComplex<T> * list = reinterpret_cast<ListComplex<T> *>(context);
      float xI = list->childAtIndex(i).toScalar();
      float xJ =  list->childAtIndex(j).toScalar();
      return Poincare::Helpers::FloatIsGreater(xI, xJ, ListSort::k_nanIsGreatest);
      },
      ctx,
      list.numberOfChildren());
  return list;
}


template Evaluation<float> ListHelpers::ExtremumApproximationOfListNode<float>(ListNode * list, ExpressionNode::ApproximationContext approximationContext, bool minimum);
template Evaluation<double> ListHelpers::ExtremumApproximationOfListNode<double>(ListNode * list, ExpressionNode::ApproximationContext approximationContext, bool minimum);

template Evaluation<float> ListHelpers::SumOfListNode<float>(ListNode * list, ExpressionNode::ApproximationContext approximationContext);
template Evaluation<double> ListHelpers::SumOfListNode<double>(ListNode * list, ExpressionNode::ApproximationContext approximationContext);

template Evaluation<float> ListHelpers::ProductOfListNode<float>(ListNode * list, ExpressionNode::ApproximationContext approximationContext);
template Evaluation<double> ListHelpers::ProductOfListNode<double>(ListNode * list, ExpressionNode::ApproximationContext approximationContext);

template Evaluation<float> ListHelpers::SquareSumOfListNode<float>(ListNode * list, ExpressionNode::ApproximationContext approximationContext);
template Evaluation<double> ListHelpers::SquareSumOfListNode<double>(ListNode * list, ExpressionNode::ApproximationContext approximationContext);

template ListComplex<float> ListHelpers::SortListComplex(ListComplex<float> list);
template ListComplex<double> ListHelpers::SortListComplex(ListComplex<double> list);

}
