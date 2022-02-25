#ifndef PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class HomogeneityStatistic final : public Chi2Statistic {
public:
  // Used in HomogeneityTableDataSource
  constexpr static int k_maxNumberOfColumns = 9;
  constexpr static int k_maxNumberOfRows = 9;

  HomogeneityStatistic();

  // Statistic
  void computeTest() override;
  bool validateInputs() override;

  // Chi2Statistic
  void setParameterAtPosition(int row, int column, double value) override;
  double parameterAtPosition(int row, int column) override;
  bool isValidParameterAtPosition(int row, int column, double p) override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override;
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; };
  int maxNumberOfRows() const override { return k_maxNumberOfRows; };

  int numberOfStatisticParameters() const override {
    return k_maxNumberOfColumns * k_maxNumberOfRows;
  }

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

private:
  // Chi2Statistic
  double expectedValue(int resultsIndex) override;
  double observedValue(int resultsIndex) override;
  int numberOfValuePairs() override;

  bool isValidParamAtIndex(int i, double p) override;

  double observedValueAtPosition(Index2D index);
  double expectedValueAtPosition(Index2D index);
  int computeDegreesOfFreedom(Index2D max);
  double * paramArray() override { return m_input; }
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
