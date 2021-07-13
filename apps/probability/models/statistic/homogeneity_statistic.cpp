#include "homogeneity_statistic.h"

namespace Probability {

void HomogeneityStatistic::setParameterAtPosition(int row, int column, float value) {
  setParamAtIndex(column + row * k_maxNumberOfColumns, value);
}

} // namespace Probability
