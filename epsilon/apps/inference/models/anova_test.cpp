#include "anova_test.h"

#include <poincare/statistics/inference.h>

namespace Inference {

void ANOVATest::compute() {
  using namespace Poincare::Inference::SignificanceTest::FTest;

  StatisticResults results = ComputeStatisticResults(m_groups.span());

  m_testCriticalValue = results.statistic;
  m_pValue = results.pValue;
}

}  // namespace Inference
