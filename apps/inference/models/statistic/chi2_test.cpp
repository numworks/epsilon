#include "chi2_test.h"

#include <poincare/k_tree.h>

#include <algorithm>
#include <cmath>

#include "goodness_test.h"
#include "homogeneity_test.h"

namespace Inference {

Chi2Test::Chi2Test() {
  m_hypothesisParams.setComparisonOperator(
      Poincare::ComparisonNode::OperatorType::Superior);  // Always higher
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
  initParameters();
  return true;
}

double Chi2Test::computeChi2() {
  double z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += computeContribution(i);
  }
  return z;
}

bool Chi2Test::authorizedParameterAtIndex(double p, int i) const {
  if (i == indexOfThreshold() && !SignificanceTest::ValidThreshold(p)) {
    return false;
  }
  return Inference::authorizedParameterAtIndex(p, i);
}

Shared::ParameterRepresentation Chi2Test::paramRepresentationAtIndex(
    int i) const {
  return Shared::ParameterRepresentation{KRackL(), I18n::Message::Default};
}

double Chi2Test::computeContribution(int i) const {
  return std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
}

}  // namespace Inference
