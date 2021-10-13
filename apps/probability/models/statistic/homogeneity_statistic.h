#ifndef PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class HomogeneityStatistic : public Chi2Statistic {
public:
  HomogeneityStatistic();
  // TODO Factorize in Chi2Statistic
  void setParameterAtPosition(int row, int column, double value);
  double parameterAtPosition(int row, int column);
  bool isValidParamAtPosition(int row, int column, double p);
  bool deleteParamAtPosition(int row, int column);
  int numberOfStatisticParameters() const override {
    return k_maxNumberOfColumns * k_maxNumberOfRows;
  }
  void computeTest() override;

  int numberOfResultRows() { return m_numberOfResultRows; }
  int numberOfResultColumns() { return m_numberOfResultColumns; }
  double expectedValueAtLocation(int row, int column);

  double total() { return m_total;}
  double rowTotal(int row) { return m_rowTotals[row]; }
  double columnTotal(int column) { return m_columnTotals[column]; }

  struct Index2D {
    int row;
    int col;
  };
  Index2D computeDimensions();

  void recomputeData();
  bool validateInputs() override;

  constexpr static int k_maxNumberOfColumns = 9;
  constexpr static int k_maxNumberOfRows = 9;

protected:
  double observedValue(int resultsIndex) override;
  double expectedValue(int resultsIndex) override;
  double observedValueAtPosition(Index2D index);
  double expectedValueAtPosition(Index2D index);
  int computeDegreesOfFreedom(Index2D max);
  int numberOfValuePairs() override;
  double * paramArray() override { return m_input; }

private:
  // TODO: factorize in Chi2Statistic
  Index2D indexToIndex2D(int index);
  int index2DToIndex(Index2D indexes);
  int index2DToIndex(int row, int column);
  Index2D resultsIndexToIndex2D(int resultsIndex);
  int resultsIndexToArrayIndex(int resultsIndex);
  void computeExpectedValues(Index2D max);

  double m_input[k_maxNumberOfColumns * k_maxNumberOfRows];
  double m_expectedValues[k_maxNumberOfColumns * k_maxNumberOfRows];
  double m_rowTotals[k_maxNumberOfRows];
  double m_columnTotals[k_maxNumberOfColumns];
  double m_total;
  int m_numberOfResultRows;
  int m_numberOfResultColumns;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H */
