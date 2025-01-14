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

  static Table* FromStatistic(Statistic*);

  // Store functions
  virtual int numberOfSeries() const = 0;
  /* Number of series to display in the table. By default it is equal to the
   * number of series of the statistics, but in some cases these two values can
   * differ (for example in TwoMeans). */
  virtual int numberOfSeriesInTable() const { return numberOfSeries(); }
  virtual int seriesAt(int index) const { return -1; }
  virtual void setSeriesAt(Statistic*, int index, int series) {}
  bool hasSeries() const;
  void unsetSeries(Statistic*);

  // Matrix special functions
  virtual void setParameterAtPosition(double value, int row, int column) = 0;
  virtual double parameterAtPosition(int row, int column) const = 0;
  virtual bool authorizedParameterAtPosition(double p, int row,
                                             int column) const = 0;
  virtual void deleteParametersInColumn(int column);
  /* Delete parameter at location, return true if the deleted param was the last
   * non-deleted value of its row or column. */
  virtual bool deleteParameterAtPosition(int row, int column);
  virtual void recomputeData() = 0;
  virtual int maxNumberOfColumns() const = 0;
  virtual int maxNumberOfRows() const = 0;

  bool validateSeries(Shared::DoublePairStore* doublePairStore,
                      int index) const;

  struct Index2D {
    int row;
    int col;
  };
  Index2D indexToIndex2D(int index) const;
  Index2D computeDimensions() const;

  constexpr static float k_undefinedValue = NAN;

 protected:
  Index2D computeInnerDimensions() const;
  virtual Index2D initialDimensions() const = 0;
  int index2DToIndex(Index2D indexes) const;
  int index2DToIndex(int row, int column) const;
};

}  // namespace Inference

#endif
