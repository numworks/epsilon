#include "t_statistic.h"

#include <poincare/normal_distribution.h>

#include "probability/models/student_law.h"

namespace Probability {

using namespace Poincare;

float TStatistic::normedDensityFunction(float x) {
  return StudentLaw::EvaluateAtAbscissa<float>(x, m_degreesOfFreedom);
}

float TStatistic::_tAlpha(float degreesOfFreedom, float alpha) {
  float x =
      m_hypothesisParams.op() == HypothesisParams::ComparisonOperator::Lower ? alpha : 1 - alpha;
  return StudentLaw::CumulativeDistributiveInverseForProbability<float>(x, degreesOfFreedom);
  ;
}

float TStatistic::_pVal(float degreesOfFreedom, float t) {
  switch (m_hypothesisParams.op()) {
    case HypothesisParams::ComparisonOperator::Higher:
      return 1 - StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(t, degreesOfFreedom);
    case HypothesisParams::ComparisonOperator::Lower:
      return StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(t, degreesOfFreedom);
    case HypothesisParams::ComparisonOperator::Different:
      assert(t > 0);
      return StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(-t, degreesOfFreedom) + 1 -
             StudentLaw::CumulativeDistributiveFunctionAtAbscissa<float>(t, degreesOfFreedom);
  }
  return -1;
}

float TStatistic::_tCritical(float degreesOfFreedom, float confidenceLevel) {
  return StudentLaw::CumulativeDistributiveInverseForProbability<float>(
      0.5 + confidenceLevel / 2, degreesOfFreedom);
}

}  // namespace Probability
