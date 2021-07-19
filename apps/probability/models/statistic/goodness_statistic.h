#ifndef APPS_PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class GoodnessStatistic : public Chi2Statistic {
public:
  GoodnessStatistic();
  int numberOfStatisticParameters() const override { return k_maxNumberOfRows * 2; }

  constexpr static int k_maxNumberOfRows = 10;
  void computeTest() override;

protected:
  float * paramArray() override { return m_input; }
  int _degreesOfFreedom() { return numberOfValuePairs() - 1; }
  float expectedValue(int index) override;
  float observedValue(int index) override;

private:
  int numberOfValuePairs() override;

  float m_input[k_maxNumberOfRows * 2];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H */
