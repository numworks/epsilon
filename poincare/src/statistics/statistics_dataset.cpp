#include <omg/float.h>
#include <omg/list.h>
#include <omg/unreachable.h>
#include <poincare/preferences.h>
#include <poincare/statistics/distribution.h>
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
T StatisticsDataset<T>::firstQuartile() const {
  if (SharedPreferences->methodForQuartiles() ==
          Preferences::MethodForQuartiles::CumulatedFrequency ||
      !areWeightsAllIntegers()) {
    return sortedElementAtCumulatedFrequency(1.0 / 4.0, false);
  }
  assert(SharedPreferences->methodForQuartiles() ==
         Preferences::MethodForQuartiles::MedianOfSublist);
  return sortedElementAtCumulatedWeight(std::floor(totalWeight() / 2.0) / 2.0,
                                        true);
}

template <typename T>
T StatisticsDataset<T>::thirdQuartile() const {
  if (SharedPreferences->methodForQuartiles() ==
          Preferences::MethodForQuartiles::CumulatedFrequency ||
      !areWeightsAllIntegers()) {
    return sortedElementAtCumulatedFrequency(3.0 / 4.0, false);
  }
  assert(SharedPreferences->methodForQuartiles() ==
         Preferences::MethodForQuartiles::MedianOfSublist);
  return sortedElementAtCumulatedWeight(
      std::ceil(3.0 / 2.0 * totalWeight()) / 2.0, true);
}

template <typename T>
T StatisticsDataset<T>::quartileRange() const {
  return thirdQuartile() - firstQuartile();
}

template <typename T>
T StatisticsDataset<T>::lowerFence() const {
  return firstQuartile() - 1.5 * quartileRange();
}

template <typename T>
T StatisticsDataset<T>::upperFence() const {
  return thirdQuartile() + 1.5 * quartileRange();
}

template <typename T>
T StatisticsDataset<T>::lowerWhisker(bool hasOutliers) const {
  return valueAtIndex(indexAtSortedIndex(lowerWhiskerSortedIndex(hasOutliers)));
}

template <typename T>
T StatisticsDataset<T>::upperWhisker(bool hasOutliers) const {
  return valueAtIndex(indexAtSortedIndex(upperWhiskerSortedIndex(hasOutliers)));
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
T StatisticsDataset<T>::min() const {
  int length = datasetLength();
  for (int k = 0; k < length; k++) {
    // Look for the first non null value
    int sortedIndex = indexAtSortedIndex(k);
    if (weightAtIndex(sortedIndex) > 0.0) {
      return valueAtIndex(sortedIndex);
    }
  }
  return DBL_MAX;
}

template <typename T>
T StatisticsDataset<T>::minIncludingNullFrequencies() const {
  return valueAtIndex(indexAtSortedIndex(0));
}

template <typename T>
T StatisticsDataset<T>::max() const {
  int length = datasetLength();
  for (int k = length - 1; k >= 0; k--) {
    // Look for the first non null value
    int sortedIndex = indexAtSortedIndex(k);
    if (weightAtIndex(sortedIndex) > 0.0) {
      return valueAtIndex(sortedIndex);
    }
  }
  return -DBL_MAX;
}

template <typename T>
T StatisticsDataset<T>::maxIncludingNullFrequencies() const {
  return valueAtIndex(indexAtSortedIndex(datasetLength() - 1));
}

template <typename T>
T StatisticsDataset<T>::sumOfValuesBetween(T lowerBound, T upperBound,
                                           bool strictUpperBound) const {
  if (lowerBound == INFINITY || upperBound == -INFINITY) {
    return 0;
  }
  bool stopIfEqual = strictUpperBound && upperBound != INFINITY;
  T result = 0;
  int numberOfPairs = datasetLength();
  for (int k = 0; k < numberOfPairs; k++) {
    int sortedIndex = indexAtSortedIndex(k);
    T value = valueAtIndex(sortedIndex);
    if (value > upperBound ||
        (stopIfEqual && OMG::Float::RelativelyEqual<T>(
                            value, upperBound, OMG::Float::EpsilonLax<T>()))) {
      break;
    }
    if (value >= lowerBound ||
        OMG::Float::RelativelyEqual<T>(value, lowerBound,
                                       OMG::Float::EpsilonLax<T>())) {
      result += weightAtIndex(sortedIndex);
    }
  }
  return result;
}

template <typename T>
void StatisticsDataset<T>::countDistinctValues(int start, int end, int i,
                                               bool handleNullFrequencies,
                                               T* value,
                                               int* distinctValues) const {
  assert(start >= 0 && end <= datasetLength() && start <= end);
  *distinctValues = 0;
  *value = NAN;
  for (int j = start; j < end; j++) {
    int valueIndex = indexAtSortedIndex(j);
    if (handleNullFrequencies || weightAtIndex(valueIndex) > 0.0) {
      T nextX = valueAtIndex(valueIndex);
      // *value != nextX returns true if *value is NAN
      if (*value != nextX) {
        (*distinctValues)++;
        *value = nextX;
      }
      if (i == (*distinctValues) - 1) {
        // Found the i-th distinct value
        return;
      }
    }
  }
  assert(i == -1);
}

template <typename T>
bool StatisticsDataset<T>::areWeightsAllIntegers() const {
  for (int i = 0; i < datasetLength(); i++) {
    T weight = weightAtIndex(indexAtSortedIndex(i));
    if (weight != std::round(weight)) {
      return false;
    }
  }
  return true;
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

template <typename T>
T StatisticsDataset<T>::heightOfBarAtIndex(int index, T barWidth,
                                           T firstDrawnBarAbscissa) const {
  return sumOfValuesBetween(
      startOfBarAtIndex(index, barWidth, firstDrawnBarAbscissa),
      endOfBarAtIndex(index, barWidth, firstDrawnBarAbscissa));
}

template <typename T>
T StatisticsDataset<T>::startOfBarAtIndex(int index, T barWidth,
                                          T firstDrawnBarAbscissa) const {
  double minValue = min();
  double firstBarAbscissa =
      firstDrawnBarAbscissa +
      barWidth * std::floor((minValue - firstDrawnBarAbscissa) / barWidth);
  /* Because of floating point approximation, firstBarAbscissa could be above
   * the minimal value, or too much below. As a result, we would compute a
   * height of zero for all bars. */
  if (firstBarAbscissa > minValue) {
    firstBarAbscissa -= barWidth;
  } else if (firstBarAbscissa + barWidth <= minValue) {
    firstBarAbscissa += barWidth;
  }
  return firstBarAbscissa + index * barWidth;
}

template <typename T>
T StatisticsDataset<T>::endOfBarAtIndex(int index, T barWidth,
                                        T firstDrawnBarAbscissa) const {
  return startOfBarAtIndex(index + 1, barWidth, firstDrawnBarAbscissa);
}

template <typename T>
int StatisticsDataset<T>::numberOfBars(T barWidth, T firstDrawnBarAbscissa) {
  T firstBarAbscissa = startOfBarAtIndex(0, barWidth, firstDrawnBarAbscissa);
  T maxValue = max();
  int nBars = static_cast<int>(
      std::floor((maxValue - firstBarAbscissa) / barWidth) + 1);
  if (OMG::Float::RelativelyEqual<T>(maxValue,
                                     firstBarAbscissa + nBars * barWidth,
                                     OMG::Float::EpsilonLax<T>())) {
    /* If the maxValue is on the upper bound of the last bar, we need to add
     * one bar to be consistent with sumOfValuesBetween. */
    nBars++;
  }
  return nBars;
}

template <typename T>
int StatisticsDataset<T>::totalCumulatedFrequencyValues() const {
  T value;
  int distinctValues;
  countDistinctValues(0, datasetLength(), -1, true, &value, &distinctValues);
  return distinctValues;
}

template <typename T>
T StatisticsDataset<T>::cumulatedFrequencyValueAtIndex(int i) const {
  T value;
  int distinctValues;
  countDistinctValues(0, datasetLength(), i, true, &value, &distinctValues);
  return value;
}

template <typename T>
T StatisticsDataset<T>::cumulatedFrequencyResultAtIndex(int i) const {
  T cumulatedOccurrences = 0.0;
  int index = 0;
  const T value = cumulatedFrequencyValueAtIndex(i);
  const int length = datasetLength();
  while (index < length && valueAtIndex(indexAtSortedIndex(index)) <= value) {
    cumulatedOccurrences += weightAtIndex(indexAtSortedIndex(index));
    index++;
  }
  // Taking advantage of sumOfOccurrences being memoized.
  return cumulatedOccurrences / totalWeight();
}

template <typename T>
T StatisticsDataset<T>::cumulatedFrequencyResultAtAbscissa(T x) const {
  if (x < minIncludingNullFrequencies()) {
    return 0.0;
  }
  if (x >= maxIncludingNullFrequencies()) {
    return 1.0;
  }

  // Find the closest indices for interpolation
  const int totalValues = totalCumulatedFrequencyValues();
  int lowerIndex = -1;
  int upperIndex = -1;

  // Find the two closest distinct values for interpolation
  for (int i = 0; i < totalValues; i++) {
    T currentValue = cumulatedFrequencyValueAtIndex(i);
    if (currentValue <= x) {
      lowerIndex = i;
    } else {
      upperIndex = i;
      break;
    }
  }

  // If x equals a value in the dataset, return the exact cumulated frequency
  if (lowerIndex >= 0 &&
      OMG::Float::RelativelyEqual<T>(cumulatedFrequencyValueAtIndex(lowerIndex),
                                     x, OMG::Float::EpsilonLax<T>())) {
    return cumulatedFrequencyResultAtIndex(lowerIndex);
  }

  // x is between two values: interpolate
  T lowerValue = cumulatedFrequencyValueAtIndex(lowerIndex);
  T upperValue = cumulatedFrequencyValueAtIndex(upperIndex);
  T lowerResult = cumulatedFrequencyResultAtIndex(lowerIndex);
  T upperResult = cumulatedFrequencyResultAtIndex(upperIndex);

  // Linear interpolation: y = y1 + (x - x1) * (y2 - y1) / (x2 - x1)
  return lowerResult + (x - lowerValue) * (upperResult - lowerResult) /
                           (upperValue - lowerValue);
}

template <typename T>
int StatisticsDataset<T>::totalNormalProbabilityValues() const {
  if (!areWeightsAllIntegers()) {
    return 0;
  }
  T result = totalWeight();
  assert(result == std::round(result));
  return static_cast<int>(result);
}

template <typename T>
T StatisticsDataset<T>::normalProbabilityValueAtIndex(int i) const {
  assert(areWeightsAllIntegers());
  return sortedElementAtCumulatedWeight(i + 1, false);
}

template <typename T>
T StatisticsDataset<T>::normalProbabilityResultAtIndex(int i) const {
  T total = static_cast<T>(totalNormalProbabilityValues());
  assert(i >= 0 && total > 0.0 && static_cast<T>(i) < total);
  // invnorm((i-0.5)/total,0,1)
  T plottingPosition = (static_cast<T>(i) + 0.5) / total;
  constexpr Poincare::Distribution::ParametersArray<T> k_distribParams = {0.0,
                                                                          1.0};
  return Poincare::Distribution::CumulativeDistributiveInverseForProbability(
      Poincare::Distribution::Type::Normal, plottingPosition, k_distribParams);
}

template <typename T>
T StatisticsDataset<T>::normalProbabilityZScoreLineAtAbscissa(T x) const {
  T meanValue = mean();
  T sigmaValue = standardDeviation();
  assert(sigmaValue != 0.0);
  return (x - meanValue) / sigmaValue;
}

template <typename T>
int StatisticsDataset<T>::lowerWhiskerSortedIndex(bool hasOutliers) const {
  T lowFence = lowerFence();
  int length = datasetLength();
  for (int k = 0; k < length; k++) {
    int valueIndex = indexAtSortedIndex(k);
    if ((!hasOutliers || valueAtIndex(valueIndex) >= lowFence) &&
        weightAtIndex(valueIndex) > 0.0) {
      return k;
    }
  }
  return length;
}

template <typename T>
int StatisticsDataset<T>::upperWhiskerSortedIndex(bool hasOutliers) const {
  T uppFence = upperFence();
  int length = datasetLength();
  for (int k = length - 1; k >= 0; k--) {
    int valueIndex = indexAtSortedIndex(k);
    if ((!hasOutliers || valueAtIndex(valueIndex) <= uppFence) &&
        weightAtIndex(valueIndex) > 0.0) {
      return k;
    }
  }
  return length;
}

template <typename T>
int StatisticsDataset<T>::numberOfLowerOutliers() const {
  T value;
  int distinctValues;
  countDistinctValues(0, lowerWhiskerSortedIndex(true), -1, false, &value,
                      &distinctValues);
  return distinctValues;
}

template <typename T>
int StatisticsDataset<T>::numberOfUpperOutliers() const {
  T value;
  int distinctValues;
  countDistinctValues(upperWhiskerSortedIndex(true) + 1, datasetLength(), -1,
                      false, &value, &distinctValues);
  return distinctValues;
}

template <typename T>
T StatisticsDataset<T>::lowerOutlierAtIndex(int index) const {
  assert(index < numberOfLowerOutliers());
  T value;
  int distinctValues;
  countDistinctValues(0, lowerWhiskerSortedIndex(true), index, false, &value,
                      &distinctValues);
  return value;
}

template <typename T>
T StatisticsDataset<T>::upperOutlierAtIndex(int index) const {
  assert(index < numberOfUpperOutliers());
  T value;
  int distinctValues;
  countDistinctValues(upperWhiskerSortedIndex(true) + 1, datasetLength(), index,
                      false, &value, &distinctValues);
  return value;
}

template class StatisticsDataset<float>;
template class StatisticsDataset<double>;

}  // namespace Poincare
