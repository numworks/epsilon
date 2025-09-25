#pragma once

#include <assert.h>
#include <poincare/statistics/dataset_column.h>
#include <stdint.h>

#include <algorithm>
#include <cmath>

/* This class is used to compute basic statistics functions on a dataset.
 *
 * === INSTANTIATION ===
 * You can build it either:
 * - with 2 columns (the values and the weights/population/frequencies.
 *   This second column needs to be positive).
 * - with 1 column (values only). In this case, all weights are equal to 1.
 *
 * === COMPLEXITY ===
 * There are two categories of methods:
 * - The ones which will always take the same time (like mean).
 * - The ones which need sorted indexes (like median).
 *
 * If you need to compute a mean, variance, standardDeviation, or any other
 * method that does not need sortedIndex, you can recreate a StatisticsDataset
 * object each time. You don't need to memoize the dataset.
 * (for example, that's what we do in Apps::Regression::Store)
 *
 * If you need to compute a median, a sortedElementAtCumulatedWeight, (or any
 * other method that needs sortedIndex) multiple times with the same datas,
 * you should memoize your dataset.
 * Indeed, the object memoizes m_sortedIndex and recomputes it only if you
 * ask it to.
 * (for example, that's what we do in Apps::Statistics::Store)
 *
 * === ENHANCEMENTS ===
 * More statistics method could be implemented here if factorization is needed.
 * */

namespace Poincare {

// TODO: merge with StatisticsDataset
class StatisticsCalculationOptions {
 public:
  StatisticsCalculationOptions()
      : m_lnOfX(false), m_lnOfY(false), m_oppositeOfY(false) {}
  StatisticsCalculationOptions(bool lnOfX, bool lnOfY, bool oppositeOfY)
      : m_lnOfX(lnOfX), m_lnOfY(lnOfY), m_oppositeOfY(oppositeOfY) {}
  bool lnOfValues(int column) const { return column == 0 ? m_lnOfX : m_lnOfY; }
  bool oppositeOfValues(int column) const {
    return column == 1 && m_oppositeOfY;
  }
  double transformValue(double value, int column) const;

 private:
  const bool m_lnOfX, m_lnOfY, m_oppositeOfY;
};

template <typename T>
class StatisticsDataset {
  friend class StatisticsDatasetFromTable;

 public:
#if TODO_PCJ
  static StatisticsDataset<T> BuildFromChildren(
      const ExpressionNode* e, const ApproximationContext& approximationContext,
      ListComplex<T> evaluationArray[]);
#endif

  StatisticsDataset(const DatasetColumn<T>* values,
                    const DatasetColumn<T>* weights, bool lnOfValues = false,
                    bool oppositeOfValues = false)
      : m_values(values),
        m_weights(weights),
        m_recomputeSortedIndex(true),
        m_memoizedTotalWeight(NAN),
        m_lnOfValues(lnOfValues),
        m_oppositeOfValues(oppositeOfValues) {}
  StatisticsDataset(const DatasetColumn<T>* values, bool lnOfValues = false,
                    bool oppositeOfValues = false)
      : StatisticsDataset(values, nullptr, lnOfValues, oppositeOfValues) {}
  StatisticsDataset() : StatisticsDataset(nullptr, nullptr, false, false) {}

  bool isUndefined() { return m_values == nullptr; }

#ifdef TARGET_POINCARE_JS
  void deleteSortedIndex() {
    if (m_sortedIndex != nullptr) {
      delete[] m_sortedIndex;
      m_sortedIndex = nullptr;
    }
  }

  // Rule of 3

  StatisticsDataset(const StatisticsDataset& other)
      : m_values(other.m_values),
        m_weights(other.m_weights),
        m_recomputeSortedIndex(other.m_recomputeSortedIndex),
        m_memoizedTotalWeight(other.m_memoizedTotalWeight),
        m_memoizedModeData(other.m_memoizedModeData),
        m_lnOfValues(other.m_lnOfValues),
        m_oppositeOfValues(other.m_oppositeOfValues) {
    deleteSortedIndex();
  }

  StatisticsDataset& operator=(const StatisticsDataset& other) {
    if (this == &other) {
      return *this;
    }
    m_values = other.m_values;
    m_weights = other.m_weights;
    m_recomputeSortedIndex = other.m_recomputeSortedIndex;
    m_memoizedTotalWeight = other.m_memoizedTotalWeight;
    m_memoizedModeData = other.m_memoizedModeData;
    m_lnOfValues = other.m_lnOfValues;
    m_oppositeOfValues = other.m_oppositeOfValues;
    deleteSortedIndex();
    return *this;
  }

  ~StatisticsDataset() { deleteSortedIndex(); }
#endif

  void setHasBeenModified() {
    m_recomputeSortedIndex = true;
    m_memoizedTotalWeight = NAN;
    m_memoizedModeData = ModeData();
#ifdef TARGET_POINCARE_JS
    deleteSortedIndex();
#endif
  }

  int indexAtSortedIndex(int i) const;

  T totalWeight() const;
  T weightedSum() const;
  T offsettedSquaredSum(T offset) const;
  T squaredSum() const { return offsettedSquaredSum(0.0); }
  // sum(value(i) - (a + b * dataset.value(i))
  T squaredSumOffsettedByLinearTransformationOfDataset(
      const StatisticsDataset<T>* dataset, double a, double b) const;

  T mean() const { return weightedSum() / totalWeight(); }
  T variance() const;
  T standardDeviation() const { return std::sqrt(variance()); }
  T sampleStandardDeviation() const;

  T firstQuartile() const;
  T thirdQuartile() const;
  T quartileRange() const;
  T lowerFence() const;
  T upperFence() const;
  T lowerWhisker(bool hasOutliers) const;
  T upperWhisker(bool hasOutliers) const;

  /* All the following methods need sortedIndex and are way faster with a
   * length < 256. */
  T sortedElementAtCumulatedFrequency(T freq, bool createMiddleElement) const;
  T sortedElementAtCumulatedWeight(T weight, bool createMiddleElement) const;
  T median() const {
    return sortedElementAtCumulatedFrequency(1.0 / 2.0, true);
  }
  int indexAtCumulatedFrequency(T freq, int* upperIndex = nullptr) const {
    assert(freq >= 0.0 && freq <= 1.0);
    return indexAtCumulatedWeight(freq * totalWeight(), upperIndex);
  }
  int indexAtCumulatedWeight(T weight, int* upperIndex = nullptr) const;
  int medianIndex(int* upperIndex = nullptr) const {
    return indexAtCumulatedFrequency(1.0 / 2.0, upperIndex);
  }

  T min() const;
  T minIncludingNullFrequencies() const;
  T max() const;
  T maxIncludingNullFrequencies() const;
  T sumOfValuesBetween(T lowerBound, T upperBound,
                       bool strictUpperBound = true) const;
  /* Find the i-th distinct value (if i is -1, browse the entire dataset) from
   * start to end (ordered by value).
   * Retrieve the i-th value and the number distinct values encountered.
   * If not handleNullFrequencies, ignore values with null frequency. */
  void countDistinctValues(int start, int end, int i,
                           bool handleNullFrequencies, T* value,
                           int* distinctValues) const;
  bool areWeightsAllIntegers() const;

  struct ModeData {
    int numberOfModes = 0;
    T modeWeight = 0.;
  };

  ModeData modeData() const;
  T modeValueAtIndex(int index) const;

  /* Histogram bars */

  T heightOfBarAtIndex(int index, T barWidth, T firstDrawnBarAbscissa) const;
  T startOfBarAtIndex(int index, T barWidth, T firstDrawnBarAbscissa) const;
  T endOfBarAtIndex(int index, T barWidth, T firstDrawnBarAbscissa) const;
  int numberOfBars(T barWidth, T firstDrawnBarAbscissa);

  /* Cumulated frequencies
   * Distinct values are aggregated and their frequency summed */

  // Return number of distinct values
  int totalCumulatedFrequencyValues() const;
  // Return the i-th distinct sorted value
  T cumulatedFrequencyValueAtIndex(int index) const;
  // Return the cumulated frequency of the i-th distinct sorted value
  T cumulatedFrequencyResultAtIndex(int index) const;
  /* Return the cumulated frequency result
   * for a given abscissa value using interpolation */
  T cumulatedFrequencyResultAtAbscissa(T x) const;

  /* Normal probability
   * Values are scattered into elements of frequency 1 */

  /* Return the totalWeight, return 0 if it exceeds k_maxNumberOfPairs or
   * if any frequency is not an integer */
  int totalNormalProbabilityValues() const;
  // Return the sorted element at cumulated population i+1
  T normalProbabilityValueAtIndex(int i) const;
  // Return the z-score of the i-th sorted element
  T normalProbabilityResultAtIndex(int i) const;
  // Return the z-score line value for a given abscissa value
  T normalProbabilityZScoreLineAtAbscissa(T x) const;

  /* Boxplot */

  int lowerWhiskerSortedIndex(bool hasOutliers) const;
  int upperWhiskerSortedIndex(bool hasOutliers) const;
  int numberOfLowerOutliers() const;
  int numberOfUpperOutliers() const;
  T lowerOutlierAtIndex(int index) const;
  T upperOutlierAtIndex(int index) const;

 private:
  int datasetLength() const {
    assert(m_weights == nullptr || m_weights->length() == m_values->length());
    return m_values->length();
  }
  T valueAtIndex(int index) const;
  T weightAtIndex(int index) const;
  T privateTotalWeight() const;

  /* If the data is invalid (numberOfModes == 0), it's recomputed.
   * If computedValueIndex and computedValue are provided, the mode at this
   * index is computed */
  void computeMode(ModeData* data, int computedValueIndex = -1,
                   T* computedValue = nullptr) const;

  void buildMemoizedSortedIndex() const;

  const DatasetColumn<T>* m_values;
  const DatasetColumn<T>* m_weights;
#ifndef TARGET_POINCARE_JS
  constexpr static int k_maxLengthForSortedIndex = 256;
  mutable uint8_t m_sortedIndex[k_maxLengthForSortedIndex];
#else
  // Use a malloced array to avoid the 256 elements limit
  mutable int* m_sortedIndex = nullptr;
#endif
  mutable bool m_recomputeSortedIndex;
  mutable double m_memoizedTotalWeight;
  mutable ModeData m_memoizedModeData;
  bool m_lnOfValues;
  bool m_oppositeOfValues;
};

}  // namespace Poincare
