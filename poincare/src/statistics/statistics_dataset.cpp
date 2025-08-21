#include <omg/float.h>
#include <omg/list.h>
#include <omg/unreachable.h>
#include <poincare/statistics/statistics_dataset.h>

#include <algorithm>
#include <cmath>

namespace Poincare {

double StatisticsCalculationOptions::transformValue(double value,
                                                    int column) const {
  value *= oppositeOfValues(column) ? -1.0 : 1.0;
  return lnOfValues(column) ? std::log(value) : value;
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
  /* Use [this] as a dummy offset dataset, values will be accessed and
   * multiplied by 0. */
  return squaredSumOffsettedByLinearTransformationOfDataset(this, offset, 0.0);
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
typename StatisticsDataset<T>::ModeData StatisticsDataset<T>::modeData() const {
  if (m_memoizedModeData.numberOfModes > 0) {
    return m_memoizedModeData;
  }
  computeMode(&m_memoizedModeData);
  return m_memoizedModeData;
}

template <typename T>
T StatisticsDataset<T>::modeValueAtIndex(int index) const {
  T result;
  computeMode(&m_memoizedModeData, index, &result);
  return result;
}

template <typename T>
void StatisticsDataset<T>::computeMode(StatisticsDataset<T>::ModeData* data,
                                       int computedValueIndex,
                                       T* computedValue) const {
  bool computeData = data->numberOfModes == 0;
  if (computeData) {
    // Ensure modeWeight is properly reset
    data->modeWeight = 0.0;
  }

  int n = datasetLength();
  int currentModeIndex = 0;
  T currentValue = NAN;
  T currentWeight = 0.0;

  for (int j = 0; j <= n; j++) {
    T value, weight;
    if (j < n) {
      int valueIndex = indexAtSortedIndex(j);
      value = valueAtIndex(valueIndex);
      weight = weightAtIndex(valueIndex);
    } else {
      // Iterating one last time to process the last value
      value = weight = NAN;
    }

    // currentValue != value returns true if currentValue or value is NAN
    if (currentValue != value) {
      // A new value has been found
      if (computeData && currentWeight > data->modeWeight) {
        // A better mode has been found, reset solutions
        data->modeWeight = currentWeight;
        data->numberOfModes = 0;
        currentModeIndex = 0;
      }
      if (currentWeight == data->modeWeight) {
        if (computeData) {
          // Another mode has been found
          data->numberOfModes += 1;
        }
        if (computedValue && currentModeIndex == computedValueIndex) {
          *computedValue = currentValue;
          if (!computeData) {
            /* Do not escape early if computeData is true, since modeWeight
             * could still change, and thus the computed value too */
            return;
          }
        }
        currentModeIndex++;
      }

      currentWeight = 0.0;
      currentValue = value;
    }

    currentWeight += weight;
  }

  // A valid total and weight have been calculated
  assert(data->numberOfModes > 0 && data->modeWeight > 0.0);
  assert(computedValueIndex < data->numberOfModes);
}

template <typename T>
int StatisticsDataset<T>::indexAtSortedIndex(int i) const {
  buildMemoizedSortedIndex();
  return static_cast<int>(m_sortedIndex[i]);
}

#ifndef TARGET_POINCARE_JS
using MemoizedIndexType = uint8_t;
#else
using MemoizedIndexType = int;
#endif

template <typename T>
void StatisticsDataset<T>::buildMemoizedSortedIndex() const {
  if (!m_recomputeSortedIndex) {
    return;
  }

  int length = datasetLength();
#ifndef TARGET_POINCARE_JS
  assert(length < k_maxLengthForSortedIndex);
#else
  assert(m_sortedIndex == nullptr);
  m_sortedIndex = new int[length];
#endif

  for (int i = 0; i < length; i++) {
    m_sortedIndex[i] = i;
  }
  void* pack[] = {&(m_sortedIndex[0]), const_cast<DatasetColumn<T>*>(m_values)};
  OMG::List::Sort(
      [](int i, int j, void* ctx, int n) {  // swap
        void** pack = reinterpret_cast<void**>(ctx);
        MemoizedIndexType* sortedIndex =
            reinterpret_cast<MemoizedIndexType*>(pack[0]);
        std::swap(sortedIndex[i], sortedIndex[j]);
      },
      [](int i, int j, void* ctx, int n) {  // compare
        void** pack = reinterpret_cast<void**>(ctx);
        MemoizedIndexType* sortedIndex =
            reinterpret_cast<MemoizedIndexType*>(pack[0]);
        DatasetColumn<T>* values = reinterpret_cast<DatasetColumn<T>*>(pack[1]);
        return std::isnan(values->valueAtIndex(sortedIndex[i])) ||
               values->valueAtIndex(sortedIndex[i]) >=
                   values->valueAtIndex(sortedIndex[j]);
      },
      pack, datasetLength());
  m_recomputeSortedIndex = false;
}

template class StatisticsDataset<float>;
template class StatisticsDataset<double>;

}  // namespace Poincare
