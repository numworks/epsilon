#ifndef APPS_PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class GoodnessStatistic : public Chi2Statistic {
public:
  GoodnessStatistic();
  int numberOfStatisticParameters() const override { return m_numberOfInputRows; }

  constexpr static int k_maxNumberOfRows = 10;

protected:
  float * paramArray() override { return m_input; }
  int _degreesOfFreedom() override { return _numberOfInputRows() - 1; }

private:
  int _numberOfInputRows();
  void computeNumberOfParameters() override;

  float m_input[k_maxNumberOfRows * 2];
  int m_numberOfInputRows;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H */
