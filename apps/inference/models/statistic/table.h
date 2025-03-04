#ifndef INFERENCE_MODELS_STATISTIC_TABLE_H
#define INFERENCE_MODELS_STATISTIC_TABLE_H

#include <apps/shared/double_pair_store.h>
#include <assert.h>

#include <cmath>

namespace Inference {

class Statistic;

class Table {
 public:
  constexpr static int k_maxNumberOfStoreColumns = 4;

  // Store functions
  virtual int numberOfSeries() const = 0;
  virtual int seriesAt(int pageIndex) const { return -1; }
  virtual void setSeriesAt(Statistic*, int pageIndex, int series) {}
  bool hasSeries(int pageIndex) const;
  bool hasAllSeries() const;
  void unsetSeries(Statistic*);

  // Matrix special functions
  virtual void setValueAtPosition(double value, int row, int column) = 0;
  virtual double valueAtPosition(int row, int column) const = 0;
  virtual bool authorizedValueAtPosition(double p, int row,
                                         int column) const = 0;
  virtual void deleteValuesInColumn(int column);
  /* Delete value at location, return true if the deleted value was the last
   * non-deleted value of its row or column. */
  virtual bool deleteValueAtPosition(int row, int column);
  virtual void recomputeData() = 0;
  virtual int maxNumberOfColumns() const = 0;
  virtual int maxNumberOfRows() const = 0;

  struct Index2D {
    int row;
    int col;
  };
  Index2D indexToIndex2D(int index) const;
  Index2D computeDimensions() const;

  constexpr static float k_undefinedValue = NAN;

 protected:
  bool validateSeries(Shared::DoublePairStore* doublePairStore,
                      int index) const;
  Index2D computeInnerDimensions() const;
  virtual Index2D initialDimensions() const = 0;
  int index2DToIndex(Index2D indexes) const;
  int index2DToIndex(int row, int column) const;
};

}  // namespace Inference

#endif
