#include "chi2_test.h"
#include "goodness_test.h"
#include "homogeneity_test.h"

#include <cmath>

namespace Probability {

Chi2Test::Chi2Test() : Test() {
  m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);  // Always higher
}

void Chi2Test::initializeCategoricalType(CategoricalType type) {
  this->~Chi2Test();
  switch (type) {
    case CategoricalType::Homogeneity:
      new (this) HomogeneityTest();
      return;
    case CategoricalType::GoodnessOfFit:
      new (this) GoodnessTest();
      return;
    default:
      assert(false);
  }
}

double Chi2Test::computeChi2() {
  double z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

void Chi2Test::setParameterAtIndex(double p, int i) {
  if (i == indexOfDegreeOfFreedom()) {
    m_degreesOfFreedom = p;
  } else {
    return Statistic::setParameterAtIndex(p, i);
  }
}

bool Chi2Test::authorizedParameterAtIndex(double p, int i) const {
  if (i == indexOfDegreeOfFreedom()) {
    return p == std::round(p) && p >= 1.0;
  }
  return Statistic::authorizedParameterAtIndex(i, p);
}

}  // namespace Probability
