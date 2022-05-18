#ifndef POINCARE_STATISTICS_DATASET_H
#define POINCARE_STATISTICS_DATASET_H

#include "dataset_column.h"
#include "list.h"
#include "list_complex.h"
#include <algorithm>

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
 * - The ones which memoize sorted indexes (like median).
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

  // Need sortedIndex
  T sortedElementAtCumulatedFrequency(T freq, bool createMiddleElement) const;
  T sortedElementAtCumulatedWeight(T weight, bool createMiddleElement) const;
  T median() const;
  int indexAtCumulatedFrequency(T freq, int * upperIndex = nullptr) const;
  int indexAtCumulatedWeight(T weight, int * upperIndex = nullptr) const;
  int medianIndex(int * upperIndex = nullptr) const;

private:
  int datasetLength() const { return m_weights == nullptr ? m_values->length() : std::max(m_values->length(), m_weights->length()); }
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
