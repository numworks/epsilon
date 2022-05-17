#include <poincare/statistics_dataset.h>
#include <poincare/based_integer.h>
#include <helpers.h>
#include <cmath>
#include <float.h>

namespace Poincare {

template<typename T>
StatisticsDataset<T> StatisticsDataset<T>::BuildFromChildren(const ExpressionNode * e, ExpressionNode::ApproximationContext approximationContext, ListComplex<T> evaluationArray[]) {
  int n = e->numberOfChildren();
  if (n == 0) {
    return StatisticsDataset<T>();
  }
  for (int i = 0; i < std::min(n, 2); i++) {
    Evaluation<T> childEval = e->childAtIndex(i)->approximate(T(), approximationContext);
    if (childEval.type() != EvaluationNode<T>::Type::ListComplex) {
      return StatisticsDataset<T>();
    }
    evaluationArray[i] = static_cast<ListComplex<T> &>(childEval);
  }
  if (n == 1) {
    return StatisticsDataset<T>(&evaluationArray[0]);
  }
  return StatisticsDataset<T>(&evaluationArray[0], &evaluationArray[1]);
}

template<typename T>
T StatisticsDataset<T>::totalWeight() const {
  T total = (T)0.0;
  for (int i = 0; i < datasetLength(); i++) {
    total += weightAtIndex(i);
  }
  return total;
}

template<typename T>
T StatisticsDataset<T>::weightedSum() const {
  T total = (T)0.0;
  for (int i = 0; i < datasetLength(); i++) {
    total += valueAtIndex(i) * weightAtIndex(i);
  }
  return total;
}

template<typename T>
T StatisticsDataset<T>::offsettedSquaredSum(T offset) const {
  T total = (T)0.0;
  for (int i = 0; i < datasetLength(); i++) {
    T offsettedValue = valueAtIndex(i) - offset;
    total += offsettedValue * offsettedValue * weightAtIndex(i);
  }
  return total;
}

template<typename T>
T StatisticsDataset<T>::mean() const {
  return weightedSum() / totalWeight();
}

template<typename T>
T StatisticsDataset<T>::variance() const {
  /* We use the Var(X) = E[(X-E[X])^2] definition instead of Var(X) = E[X^2] - E[X]^2
   * to ensure a positive result and to minimize rounding errors */
  return offsettedSquaredSum(mean()) / totalWeight();
}

template<typename T>
T StatisticsDataset<T>::standardDeviation() const {
  return std::sqrt(variance());
}

template<typename T>
T StatisticsDataset<T>::sampleStandardDeviation() const {
  T weight = totalWeight();
  return std::sqrt(weight / (weight - (T)1.0)) * standardDeviation();
}

template<typename T>
T StatisticsDataset<T>::sortedElementAtCumulatedFrequency(T freq, bool createMiddleElement) const {
  assert(freq >= 0.0 && freq <= 1.0);
  return sortedElementAtCumulatedWeight(freq * totalWeight(), createMiddleElement);
}

template<typename T>
T StatisticsDataset<T>::sortedElementAtCumulatedWeight(T weight, bool createMiddleElement) const {
  T epsilon = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  int elementSortedIndex;
  T cumulatedWeight = (T)0.0;
  for (int i = 0; i < datasetLength(); i++) {
    elementSortedIndex = i;
    cumulatedWeight += weightAtIndex(indexAtSortedIndex(i));
    if (cumulatedWeight >= weight - epsilon) {
      break;
    }
  }
  if (createMiddleElement && std::fabs(cumulatedWeight - weight) < epsilon) {
    /* There is an element of cumulated weight, so the result is
     * the mean between this element and the next element (in terms of cumulated
     * weight) that has a non-null weight. */
    for (int i = elementSortedIndex + 1; i < datasetLength(); i++) {
      int nextElementSortedIndex = indexAtSortedIndex(i);
      if (weightAtIndex(nextElementSortedIndex) > (T)0.0) {
        return (valueAtIndex(indexAtSortedIndex(elementSortedIndex)) + valueAtIndex(nextElementSortedIndex)) / (T)2.0;
      }
    }
  }
  return valueAtIndex(indexAtSortedIndex(elementSortedIndex));
}

template<typename T>
T StatisticsDataset<T>::median() const {
  return sortedElementAtCumulatedFrequency((T)1.0/(T)2.0, true);
}

int getIntFromBasedInteger(Expression e) {
  assert(e.type() == ExpressionNode::Type::BasedInteger);
  return static_cast<BasedInteger &>(e).integer().extractedInt();
}

template<typename T>
int StatisticsDataset<T>::indexAtSortedIndex(int i) const {
  buildSortedIndex();
  return getIntFromBasedInteger(m_sortedIndex.childAtIndex(i));
}

template<typename T>
void StatisticsDataset<T>::buildSortedIndex() const {
  if (!m_recomputeSortedIndex) {
    return;
  }
  List sortedIndexes = List::Builder();
  for (int i = 0; i < datasetLength(); i++) {
    sortedIndexes.addChildAtIndexInPlace(BasedInteger::Builder(Integer(i)), i, i);
  }
  void * pack[] = {&sortedIndexes, m_values};
  Helpers::Sort(
      [](int i, int j, void * ctx, int n) { // swap
        void ** pack = reinterpret_cast<void **>(ctx);
        List * sortedIndex = reinterpret_cast<List *>(pack[0]);
        sortedIndex->swapChildrenInPlace(i, j);
      },
      [](int i, int j, void * ctx, int n) { // compare
        void ** pack = reinterpret_cast<void **>(ctx);
        List * sortedIndex = reinterpret_cast<List *>(pack[0]);
        DatasetColumn<T> * values = reinterpret_cast<DatasetColumn<T> *>(pack[1]);
        int sortedIndexI = getIntFromBasedInteger(sortedIndex->childAtIndex(i));
        int sortedIndexJ = getIntFromBasedInteger(sortedIndex->childAtIndex(j));
        return values->valueAtIndex(sortedIndexI) >= values->valueAtIndex(sortedIndexJ);
      },
      pack,
      datasetLength());
  m_recomputeSortedIndex = false;
}

template class StatisticsDataset<float>;
template class StatisticsDataset<double>;

}
