#include "chi2_test.h"
#include "goodness_test.h"
#include "homogeneity_test.h"
#include "interfaces/significance_tests.h"

#include <algorithm>
#include <cmath>

namespace Inference {

Chi2Test::Chi2Test() : Test() {
  m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);  // Always higher
}

bool Chi2Test::initializeCategoricalType(CategoricalType type) {
  if (type == categoricalType()) {
    return false;
  }
  this->~Chi2Test();
  switch (type) {
    case CategoricalType::Homogeneity:
      new (this) HomogeneityTest();
      break;
    case CategoricalType::GoodnessOfFit:
      new (this) GoodnessTest();
      break;
    default:
      assert(false);
  }
  return true;
}

double Chi2Test::computeChi2() {
  double z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

bool Chi2Test::authorizedParameterAtIndex(double p, int i) const {

  if (i == indexOfThreshold() && !SignificanceTest::ValidThreshold(p)) {
    return false;
  }
  return Inference::authorizedParameterAtIndex(p, i);
}

} // namespace Inference
