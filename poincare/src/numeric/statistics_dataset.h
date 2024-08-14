#ifndef POINCARE_NUMERIC_STATISTICS_DATASET_H
#define POINCARE_NUMERIC_STATISTICS_DATASET_H

#include <assert.h>
#include <stdint.h>

#include <algorithm>
#include <cmath>

#include "statistics_dataset_column.h"

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
 * WARNING: The main drawback of the memoization is that it's limited to 256
 * elements. If you need to compute a median on a dataset with more than 256
 * elements, you can still use a StatisticsDataset, but it will be way slower,
 * as the sorted index won't be memoized.
 *
 * === ENHANCEMENTS ===
 * More statistics method could be implemented here if factorization is needed.
 * */

namespace Poincare::Internal {

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

  ~StatisticsDataset() { deleteSortedIndex(); }
#endif

  void setHasBeenModified() {
    m_recomputeSortedIndex = true;
    m_memoizedTotalWeight = NAN;
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

  T min() const { return valueAtIndex(indexAtSortedIndex(0)); }
  T max() const {
    return valueAtIndex(indexAtSortedIndex(datasetLength() - 1));
  }

 private:
  int datasetLength() const {
    assert(m_weights == nullptr || m_weights->length() == m_values->length());
    return m_values->length();
  }
  T valueAtIndex(int index) const;
  T weightAtIndex(int index) const;
  T privateTotalWeight() const;

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
  bool m_lnOfValues;
  bool m_oppositeOfValues;
};

}  // namespace Poincare::Internal

#endif
