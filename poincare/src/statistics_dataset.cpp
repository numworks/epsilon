#include <poincare/statistics_dataset.h>
#include <poincare/based_integer.h>
#include <helpers.h>
#include <algorithm>
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
  if (n > 1 && evaluationArray[0].numberOfChildren() != evaluationArray[1].numberOfChildren()) {
    return StatisticsDataset<T>();
  }
  return n == 1 ? StatisticsDataset<T>(&evaluationArray[0]) : StatisticsDataset<T>(&evaluationArray[0], &evaluationArray[1]);
}

template<typename T>
T StatisticsDataset<T>::valueAtIndex(int index) const {
  assert(index >= 0 && index < m_values->length());
  return m_lnOfValues ? log(m_values->valueAtIndex(index)) : m_values->valueAtIndex(index);
}

template<typename T>
T StatisticsDataset<T>::weightAtIndex(int index) const {
  assert(m_weights == nullptr || (index >= 0 && index < m_weights->length()));
  if (std::isnan(valueAtIndex(index))) {
    return NAN;
  }
  if (m_weights == nullptr) {
    return 1.0;
  }
  // All weights must be positive.
  return m_weights->valueAtIndex(index) >= 0.0 ? m_weights->valueAtIndex(index) : NAN;
}

template<typename T>
T StatisticsDataset<T>::totalWeight() const {
  if (datasetLength() == 0) {
    return NAN;
  }
  T total = 0.0;
  for (int i = 0; i < datasetLength(); i++) {
    total += weightAtIndex(i);
  }
  return total;
}

template<typename T>
T StatisticsDataset<T>::weightedSum() const {
  T total = 0.0;
  for (int i = 0; i < datasetLength(); i++) {
    total += valueAtIndex(i) * weightAtIndex(i);
  }
  return total;
}

template<typename T>
T StatisticsDataset<T>::offsettedSquaredSum(T offset) const {
  T total = 0.0;
  for (int i = 0; i < datasetLength(); i++) {
    T offsettedValue = valueAtIndex(i) - offset;
    total += offsettedValue * offsettedValue * weightAtIndex(i);
  }
  return total;
}

template<typename T>
T StatisticsDataset<T>::sampleStandardDeviation() const {
  T weight = totalWeight();
  return std::sqrt(weight / (weight - 1.0)) * standardDeviation();
}

template<typename T>
T StatisticsDataset<T>::sortedElementAtCumulatedFrequency(T freq, bool createMiddleElement) const {
  assert(freq >= 0.0 && freq <= 1.0);
  return sortedElementAtCumulatedWeight(freq * totalWeight(), createMiddleElement);
}

template<typename T>
T StatisticsDataset<T>::sortedElementAtCumulatedWeight(T weight, bool createMiddleElement) const {
  int upperIndex;
  int lowerIndex = indexAtCumulatedWeight(weight, &upperIndex);
  if (createMiddleElement && upperIndex != lowerIndex) {
    return (valueAtIndex(lowerIndex) + valueAtIndex(upperIndex)) / 2.0;
  }
  return valueAtIndex(lowerIndex);
}

template<typename T>
int StatisticsDataset<T>::indexAtCumulatedWeight(T weight, int * upperIndex) const {
  if (std::isnan(weight)) {
    if (upperIndex) {
      *upperIndex = -1;
    }
    return -1;
  }
  T epsilon = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  int elementSortedIndex = -1;
  T cumulatedWeight = 0.0;
  for (int i = 0; i < datasetLength(); i++) {
    elementSortedIndex = i;
    cumulatedWeight += weightAtIndex(indexAtSortedIndex(i));
    if (cumulatedWeight >= weight - epsilon) {
      break;
    }
  }
  if (std::fabs(cumulatedWeight - weight) < epsilon) {
    /* There is an element of cumulated weight, so the result is
     * the mean between this element and the next element (in terms of cumulated
     * weight) that has a non-null weight. */
    for (int i = elementSortedIndex + 1; i < datasetLength(); i++) {
      int nextElementIndex = indexAtSortedIndex(i);
      T nextWeight = weightAtIndex(nextElementIndex);
      if (!std::isnan(nextWeight) && nextWeight > 0.0) {
        if (upperIndex) {
          *upperIndex = nextElementIndex;
        }
        return indexAtSortedIndex(elementSortedIndex);
      }
    }
  }
  if (upperIndex) {
    *upperIndex = indexAtSortedIndex(elementSortedIndex);
  }
  return indexAtSortedIndex(elementSortedIndex);
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
  void * pack[] = {&sortedIndexes, const_cast<DatasetColumn<T> *>(m_values)};
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
        return std::isnan(values->valueAtIndex(sortedIndexI)) || values->valueAtIndex(sortedIndexI) >= values->valueAtIndex(sortedIndexJ);
      },
      pack,
      datasetLength());
  m_sortedIndex = sortedIndexes;
  m_recomputeSortedIndex = false;
}

template class StatisticsDataset<float>;
template class StatisticsDataset<double>;

}
