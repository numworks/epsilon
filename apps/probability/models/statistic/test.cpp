#include "test.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <new>
#include <float.h>

#include "goodness_test.h"
#include "one_mean_t_test.h"
#include "one_proportion_z_test.h"
#include "two_means_t_test.h"
#include "two_proportions_z_test.h"

namespace Probability {

void Test::initializeSignificanceTest(SignificanceTestType testType) {
  this->~Test();
  switch (testType) {
    case SignificanceTestType::OneMean:
      new (this) OneMeanTTest();
      break;
    case SignificanceTestType::TwoMeans:
      new (this) TwoMeansTTest();
      break;
    case SignificanceTestType::OneProportion:
      new (this) OneProportionZTest();
      break;
    case SignificanceTestType::TwoProportions:
      new (this) TwoProportionsZTest();
      break;
    case SignificanceTestType::Categorical:
      new (this) GoodnessTest();
      break;
    default:
      assert(false);
      break;
  }
  initParameters();
}

void Test::compute() {
  computeTest();
  computeCurveViewRange();
}

bool Test::canRejectNull() {
  assert(m_threshold >= 0 && m_threshold <= 1);
  return pValue() <= m_threshold;
}

}  // namespace Probability
