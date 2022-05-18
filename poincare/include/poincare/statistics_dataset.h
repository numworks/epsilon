#ifndef POINCARE_STATISTICS_DATASET_H
#define POINCARE_STATISTICS_DATASET_H

#include "dataset_column.h"
#include "list.h"
#include "list_complex.h"
#include <algorithm>

namespace Poincare {

template<typename T>
class StatisticsDataset {
public:
  static StatisticsDataset<T> BuildFromChildren(const ExpressionNode * e, ExpressionNode::ApproximationContext approximationContext, ListComplex<T> evaluationArray[]);

  StatisticsDataset(const DatasetColumn<T> * values, const DatasetColumn<T> * weights) : m_values(values), m_weights(weights), m_sortedIndex(List::Builder()), m_recomputeSortedIndex(true), m_lnOfValues(false) {}
  StatisticsDataset(const DatasetColumn<T> * values) : StatisticsDataset(values, nullptr) {}
  StatisticsDataset() : StatisticsDataset(nullptr, nullptr) {}

  bool isUndefined() { return m_values == nullptr; }

  void recomputeSortedIndex() { m_recomputeSortedIndex = true; }
  int indexAtSortedIndex(int i) const;

  void setLnOfValues(bool b) { m_lnOfValues = b; }

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
  int indexAtCumulatedFrequency(T freq, int * upperIndex = nullptr) const;
  int indexAtCumulatedWeight(T weight, int * upperIndex = nullptr) const;
  int medianIndex(int * upperIndex = nullptr) const;

private:
  int datasetLength() const { return m_weights == nullptr ? m_values->length() : std::min(m_values->length(), m_weights->length()); }
  T valueAtIndex(int index) const;
  T weightAtIndex(int index) const;
  void buildSortedIndex() const;

  const DatasetColumn<T> * m_values;
  const DatasetColumn<T> * m_weights;
  mutable List m_sortedIndex;
  mutable bool m_recomputeSortedIndex;
  bool m_lnOfValues;
};

}

#endif
