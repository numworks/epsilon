#ifndef PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class GoodnessStatistic : public Chi2Statistic {
public:
  GoodnessStatistic();
  int numberOfStatisticParameters() const override { return k_maxNumberOfRows * 2; }

  void computeTest() override;
  void recomputeData();
  int computeNumberOfRows();
  bool validateInputs() override;

  double paramAtLocation(int row, int column);
  void setParamAtLocation(int row, int column, double p);
  bool isValidParamAtLocation(int row, int column, double p);

  constexpr static int k_maxNumberOfRows = 10;

private:
  bool isValidParamAtIndex(int i, double p) override;
  double * paramArray() override { return m_input; }
  int computeDegreesOfFreedom() { return numberOfValuePairs() - 1; }
  double expectedValue(int index) override;
  double observedValue(int index) override;

  void setExpectedValue(int index, double value);
  void setObservedValue(int index, double value);
  int numberOfValuePairs() override;

  int locationToTableIndex(int row, int column);

  double m_input[k_maxNumberOfRows * 2];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H */
