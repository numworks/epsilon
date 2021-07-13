#include "homogeneity_statistic.h"

namespace Probability {

void HomogeneityStatistic::setParameterAtPosition(int row, int column, float value) {
  setParamAtIndex(column + row * k_maxNumberOfColumns, value);
}

float HomogeneityStatistic::parameterAtPosition(int row, int column) {
  return paramAtIndex(column + row * k_maxNumberOfColumns);
}

} // namespace Probability
