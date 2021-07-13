#include "goodness_statistic.h"

namespace Probability {

GoodnessStatistic::GoodnessStatistic() : m_numberOfInputRows(k_maxNumberOfRows) {
  for (int i = 0; i < k_maxNumberOfRows * 2; i++) {
    m_input[i] = k_undefinedValue;
  }
}


int GoodnessStatistic::_numberOfInputRows() {
  // Compute number of rows based on undefined flag
  int i = k_maxNumberOfRows - 1;
  while (i >= 0 && std::isnan(expectedValue(i)) && std::isnan(observedValue(i))) {
    i--;
  }
  return i + 1;
}

void GoodnessStatistic::computeNumberOfParameters() {
  m_numberOfInputRows = _numberOfInputRows();
}

}  // namespace Probability
