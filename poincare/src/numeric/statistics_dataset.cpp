#include "statistics_dataset.h"

#include <omg/float.h>
#include <poincare/old/helpers.h>

#include <algorithm>
#include <cmath>

namespace Poincare::Internal {

double StatisticsCalculationOptions::transformValue(double value,
                                                    int column) const {
  value *= oppositeOfValue(column) ? -1.0 : 1.0;
  return lnOfValue(column) ? std::log(value) : value;
}

#if TODO_PCJ
template <typename T>
StatisticsDataset<T> StatisticsDataset<T>::BuildFromChildren(
    const ExpressionNode* e, const ApproximationContext& approximationContext,
    ListComplex<T> evaluationArray[]) {
  int n = e->numberOfChildren();
  if (n == 0) {
    return StatisticsDataset<T>();
  }
  for (int i = 0; i < std::min(n, 2); i++) {
    Evaluation<T> childEval =
        e->childAtIndex(i)->approximate(T(), approximationContext);
    if (childEval.otype() != EvaluationNode<T>::Type::ListComplex) {
      return StatisticsDataset<T>();
    }
    evaluationArray[i] = static_cast<ListComplex<T>&>(childEval);
  }
  if (n > 1 && evaluationArray[0].numberOfChildren() !=
                   evaluationArray[1].numberOfChildren()) {
    return StatisticsDataset<T>();
  }
  return n == 1
             ? StatisticsDataset<T>(&evaluationArray[0])
             : StatisticsDataset<T>(&evaluationArray[0], &evaluationArray[1]);
}
#endif

template <typename T>
T StatisticsDataset<T>::valueAtIndex(int index) const {
  assert(index >= 0 && index < m_values->length());
  T sign = m_oppositeOfValues ? -1.0 : 1.0;
  return m_lnOfValues ? log(sign * m_values->valueAtIndex(index))
                      : sign * m_values->valueAtIndex(index);
}

template <typename T>
T StatisticsDataset<T>::weightAtIndex(int index) const {
  assert(m_weights == nullptr || (index >= 0 && index < m_weights->length()));
  if (std::isnan(valueAtIndex(index))) {
    return NAN;
  }
  if (m_weights == nullptr) {
    return 1.0;
  }
  // All weights must be positive.
  return m_weights->valueAtIndex(index) >= 0.0 ? m_weights->valueAtIndex(index)
                                               : NAN;
}

template <typename T>
T StatisticsDataset<T>::totalWeight() const {
  if (std::isnan(m_memoizedTotalWeight)) {
    m_memoizedTotalWeight = privateTotalWeight();
  } else {
    assert(m_memoizedTotalWeight == privateTotalWeight());
  }
  return m_memoizedTotalWeight;
}

template <typename T>
T StatisticsDataset<T>::privateTotalWeight() const {
  if (datasetLength() == 0) {
    return NAN;
  }
  T total = 0.0;
  int length = datasetLength();
  for (int i = 0; i < length; i++) {
    total += weightAtIndex(i);
  }
  return total;
}

template <typename T>
T StatisticsDataset<T>::weightedSum() const {
  T total = 0.0;
  int length = datasetLength();
  for (int i = 0; i < length; i++) {
    total += valueAtIndex(i) * weightAtIndex(i);
  }
  return total;
}

template <typename T>
T StatisticsDataset<T>::offsettedSquaredSum(T offset) const {
  ConstantDatasetColumn<T> offsetColumn(offset, datasetLength());
  StatisticsDataset<T> offsetDataset(&offsetColumn);
  return squaredSumOffsettedByLinearTransformationOfDataset(&offsetDataset, 0.0,
                                                            1.0);
}

template <typename T>
T StatisticsDataset<T>::squaredSumOffsettedByLinearTransformationOfDataset(
    const StatisticsDataset<T>* dataset, double a, double b) const {
  assert(dataset->datasetLength() == datasetLength());
  T total = 0.0;
  int length = datasetLength();
  for (int i = 0; i < length; i++) {
    T offsettedValue = valueAtIndex(i) - (a + b * dataset->valueAtIndex(i));
    total += offsettedValue * offsettedValue * weightAtIndex(i);
  }
  return total;
}

template <typename T>
T StatisticsDataset<T>::variance() const {
  /* We use the Var(X) = E[(X-E[X])^2] definition instead of Var(X) = E[X^2] -
   * E[X]^2 to ensure a positive result and to minimize rounding errors */
  T m = mean();
  T v = offsettedSquaredSum(m) / totalWeight();
  return std::abs(v / m) < OMG::Float::EpsilonLax<double>() ? 0.0 : v;
}

template <typename T>
T StatisticsDataset<T>::sampleStandardDeviation() const {
  T weight = totalWeight();
  return std::sqrt(weight / (weight - 1.0)) * standardDeviation();
}

template <typename T>
T StatisticsDataset<T>::sortedElementAtCumulatedFrequency(
    T freq, bool createMiddleElement) const {
  assert(freq >= 0.0 && freq <= 1.0);
  return sortedElementAtCumulatedWeight(freq * totalWeight(),
                                        createMiddleElement);
}

template <typename T>
T StatisticsDataset<T>::sortedElementAtCumulatedWeight(
    T weight, bool createMiddleElement) const {
  int upperIndex;
  int lowerIndex = indexAtCumulatedWeight(weight, &upperIndex);
  if (lowerIndex < 0) {
    return NAN;
  }
  if (createMiddleElement && upperIndex != lowerIndex) {
    return (valueAtIndex(lowerIndex) + valueAtIndex(upperIndex)) / 2.0;
  }
  return valueAtIndex(lowerIndex);
}

template <typename T>
int StatisticsDataset<T>::indexAtCumulatedWeight(T weight,
                                                 int* upperIndex) const {
  if (std::isnan(weight)) {
    if (upperIndex) {
      *upperIndex = -1;
    }
    return -1;
  }
  T epsilon = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  int length = datasetLength();
  int elementSortedIndex = -1;
  int elementIndex = -1;
  T elementWeight = 0.0;
  T cumulatedWeight = 0.0;
  for (int i = 0; i < length; i++) {
    elementSortedIndex = i;
    elementIndex = indexAtSortedIndex(i);
    elementWeight = weightAtIndex(elementIndex);
    if (elementWeight == static_cast<T>(0.0)) {
      continue;
    }
    cumulatedWeight += elementWeight;
    if (cumulatedWeight >= weight - epsilon) {
      break;
    }
  }
  if (upperIndex) {
    *upperIndex = elementIndex;
    if (std::fabs(cumulatedWeight - weight) < epsilon) {
      /* There is an element of cumulated weight, so the result is
       * the mean between this element and the next element (in terms of
       * cumulated weight) that has a non-null weight. */
      for (int i = elementSortedIndex + 1; i < length; i++) {
        int nextElementIndex = indexAtSortedIndex(i);
        T nextWeight = weightAtIndex(nextElementIndex);
        if (!std::isnan(nextWeight) && nextWeight > 0.0) {
          *upperIndex = nextElementIndex;
          break;
        }
      }
    }
  }
  return elementIndex;
}

template <typename T>
int StatisticsDataset<T>::indexAtSortedIndex(int i) const {
  if (datasetLength() < k_maxLengthForSortedIndex) {
    buildMemoizedSortedIndex();
    return static_cast<int>(m_sortedIndex[i]);
  }
  return nonMemoizedIndexAtSortedIndex(i);
}

template <typename T>
void StatisticsDataset<T>::buildMemoizedSortedIndex() const {
  assert(datasetLength() < k_maxLengthForSortedIndex);
  if (!m_recomputeSortedIndex) {
    return;
  }
  int length = datasetLength();
  for (int i = 0; i < length; i++) {
    m_sortedIndex[i] = i;
  }
  void* pack[] = {&m_sortedIndex, const_cast<DatasetColumn<T>*>(m_values)};
  Helpers::Sort(
      [](int i, int j, void* ctx, int n) {  // swap
        void** pack = reinterpret_cast<void**>(ctx);
        uint8_t* sortedIndex = reinterpret_cast<uint8_t*>(pack[0]);
        std::swap(sortedIndex[i], sortedIndex[j]);
      },
      [](int i, int j, void* ctx, int n) {  // compare
        void** pack = reinterpret_cast<void**>(ctx);
        uint8_t* sortedIndex = reinterpret_cast<uint8_t*>(pack[0]);
        DatasetColumn<T>* values = reinterpret_cast<DatasetColumn<T>*>(pack[1]);
        return std::isnan(values->valueAtIndex(sortedIndex[i])) ||
               values->valueAtIndex(sortedIndex[i]) >=
                   values->valueAtIndex(sortedIndex[j]);
      },
      pack, datasetLength());
  m_recomputeSortedIndex = false;
}

template <typename T>
int StatisticsDataset<T>::nonMemoizedIndexAtSortedIndex(int index) const {
  int length = datasetLength();
  assert(index < length && index >= 0);

  if (length == 1) {
    return 0;
  }

  for (int i = 0; i < length; i++) {
    T candidate = valueAtIndex(i);
    bool candidateIsNaN = std::isnan(candidate);
    int numberOfSmallerValues = 0;
    for (int j = 0; j < length; j++) {
      if (j == i) {
        continue;
      }
      T valJ = valueAtIndex(j);
      bool valJIsNaN = std::isnan(valJ);
      if ((!valJIsNaN && (candidateIsNaN || valJ < candidate)) ||
          (j < i && ((valJIsNaN && candidateIsNaN) || valJ == candidate))) {
        numberOfSmallerValues++;
      }
    }
    if (numberOfSmallerValues == index) {
      return i;
    }
  }
  assert(false);
  return -1;
}

template class StatisticsDataset<float>;
template class StatisticsDataset<double>;

}  // namespace Poincare::Internal
