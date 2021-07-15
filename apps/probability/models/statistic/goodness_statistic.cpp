#include "goodness_statistic.h"

namespace Probability {

GoodnessStatistic::GoodnessStatistic() {
  for (int i = 0; i < k_maxNumberOfRows * 2; i++) {
    m_input[i] = k_undefinedValue;
  }
}

int GoodnessStatistic::numberOfValuePairs() {
  // Compute number of rows based on undefined flag
  int i = k_maxNumberOfRows - 1;
  while (i >= 0 && std::isnan(expectedValue(i)) && std::isnan(observedValue(i))) {
    i--;
  }
  return i + 1;
}

float GoodnessStatistic::expectedValue(int index) {
  return paramArray()[2 * index + 1];
}

float GoodnessStatistic::observedValue(int index) {
  return paramArray()[2 * index];
}

}  // namespace Probability
