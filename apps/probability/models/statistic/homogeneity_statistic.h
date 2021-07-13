#ifndef APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class HomogeneityStatistic : public Chi2Statistic {
public:
  void setParameterAtPosition(int row, int column, float value);

  constexpr static int k_maxNumberOfColumns = 4;
  constexpr static int k_maxNumberOfRows = k_maxNumberOfParameters / k_maxNumberOfColumns;
private:
};

} // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_HOMOGENEITY_STATISTIC_H */
