#ifndef PROBABILITY_MODELS_STATISTIC_TABLE_H
#define PROBABILITY_MODELS_STATISTIC_TABLE_H

#include <assert.h>
#include <cmath>

namespace Inference {

class Table {
public:
  // Matrix special functions
  void setParameterAtPosition(double value, int row, int column) {
    assert(index2DToIndex(row, column) < numberOfTableParameters());
    tableSetParameterAtIndex(value, index2DToIndex(row, column));
  }
  double parameterAtPosition(int row, int column) const { return tableParameterAtIndex(index2DToIndex(row, column)); }
  bool authorizedParameterAtPosition(double p, int row, int column) const { return tableAuthorizedParameterAtIndex(p, index2DToIndex(row, column)); }
  void deleteParametersInColumn(int column);
  /* Delete parameter at location, return true if the deleted param was the last
   * non-deleted value of its row or column. */
  virtual bool deleteParameterAtPosition(int row, int column);
  virtual void recomputeData() = 0;
  virtual int maxNumberOfColumns() const = 0;
  virtual int maxNumberOfRows() const = 0;

  struct Index2D {
    int row;
    int col;
  };
  Index2D computeDimensions() const;

  constexpr static float k_undefinedValue = NAN;

protected:
  Index2D computeInnerDimensions() const;
  virtual Index2D initialDimensions() const = 0;

  Index2D indexToIndex2D(int index) const;
  int index2DToIndex(Index2D indexes) const;
  int index2DToIndex(int row, int column) const;

private:
  virtual void tableSetParameterAtIndex(double p, int index) = 0;
  virtual double tableParameterAtIndex(int index) const = 0;
  virtual bool tableAuthorizedParameterAtIndex(double p, int i) const = 0;
  virtual int numberOfTableParameters() const = 0;
};

}  // namespace Inference

#endif /* PROBABILITY_MODELS_STATISTIC_CHI2_TEST_H */
