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

  constexpr static int k_maxNumberOfColumns = 8;
  constexpr static int k_maxNumberOfRows = 8;

  struct Index2D {
    int row;
    int col;
  };
protected:
  float observedValue(int index) override;
  float expectedValue(int index) override;
  float observedValueAtPosition(Index2D index);
  float expectedValueAtPosition(Index2D index);
  int _degreesOfFreedom() override;
  Index2D _numberOfInputParams();
  int numberOfValuePairs() override;

private:
  float * paramArray() override { return m_input; }

  Index2D indexToTableIndex(int index);
  int index2DToIndex(Index2D indexes);
  int index2DToIndex(int row, int column);

  float m_input[k_maxNumberOfColumns * k_maxNumberOfRows];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H */
