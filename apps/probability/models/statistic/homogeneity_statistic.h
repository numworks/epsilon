#ifndef APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class HomogeneityStatistic : public Chi2Statistic {
public:
  HomogeneityStatistic();
  void setParameterAtPosition(int row, int column, float value);
  float parameterAtPosition(int row, int column);
  int numberOfStatisticParameters() const override {
    return k_maxNumberOfColumns * k_maxNumberOfRows;
  }
  void computeTest() override;

  int numberOfResultRows() { return m_numberOfResultRows; }
  int numberOfResultColumns() { return m_numberOfResultColumns; }
  float expectedValueAtLocation(int row, int column);

  float total() { return m_total;}
  float rowTotal(int row) { return m_rowTotals[row]; }
  float columnTotal(int column) { return m_columnTotals[column]; }

  struct Index2D {
    int row;
    int col;
  };
  Index2D computeDimensions();

  void recomputeData();
  bool validateInputs() override;

  constexpr static int k_maxNumberOfColumns = 10;
  constexpr static int k_maxNumberOfRows = 10;

protected:
  float observedValue(int resultsIndex) override;
  float expectedValue(int resultsIndex) override;
  float observedValueAtPosition(Index2D index);
  float expectedValueAtPosition(Index2D index);
  int computeDegreesOfFreedom(Index2D max);
  int numberOfValuePairs() override;
  float * paramArray() override { return m_input; }

private:
  Index2D indexToIndex2D(int index);
  int index2DToIndex(Index2D indexes);
  int index2DToIndex(int row, int column);
  Index2D resultsIndexToIndex2D(int resultsIndex);
  int resultsIndexToArrayIndex(int resultsIndex);
  void computeExpectedValues(Index2D max);

  float m_input[k_maxNumberOfColumns * k_maxNumberOfRows];
  float m_expectedValues[k_maxNumberOfColumns * k_maxNumberOfRows];
  float m_rowTotals[k_maxNumberOfRows];
  float m_columnTotals[k_maxNumberOfColumns];
  float m_total;
  int m_numberOfResultRows;
  int m_numberOfResultColumns;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H */
