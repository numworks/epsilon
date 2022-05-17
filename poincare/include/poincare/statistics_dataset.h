#ifndef POINCARE_STATISTICS_DATASET_H
#define POINCARE_STATISTICS_DATASET_H

#include "dataset_column.h"
#include "list.h"
#include <algorithm>

namespace Poincare {

template<typename T>
class StatisticsDataset {
public:
  StatisticsDataset(DatasetColumn<T> * values, DatasetColumn<T> * weights) : m_values(values), m_weights(weights), m_sortedIndex(List::Builder()), m_recomputeSortedIndex(true) {}
  StatisticsDataset(DatasetColumn<T> * values) : StatisticsDataset(values, nullptr) {}

  void recomputeSortedIndex() { m_recomputeSortedIndex = true; }
  int indexAtSortedIndex(int i) const;

  T totalWeight() const;
  T weightedSum() const;
  T offsettedSquaredSum(T offset) const;
  T squaredSum() const { return offsettedSquaredSum((T)0.0);}

  T mean() const;
  T variance() const;
  T standardDeviation() const;
  T sampleStandardDeviation() const;

  T sortedElementAtCumulatedFrequency(T freq, bool createMiddleElement) const;
  T sortedElementAtCumulatedWeight(T weight, bool createMiddleElement) const;
  T median() const;

private:
  int datasetLength() const { return m_weights == nullptr ? m_values->length() : std::min(m_values->length(), m_weights->length()); }
  T valueAtIndex(int index) const {
    return index < m_values->length() ? m_values->valueAtIndex(index) : NAN;
  }
  T weightAtIndex(int index) const {
    if (m_weights == nullptr) { return (T)1.0; }
    return index < m_weights->length() ? m_weights->valueAtIndex(index) : NAN;
  }

  void buildSortedIndex() const;

  DatasetColumn<T> * m_values;
  DatasetColumn<T> * m_weights;
  mutable List m_sortedIndex;
  mutable bool m_recomputeSortedIndex;
};

}

#endif
