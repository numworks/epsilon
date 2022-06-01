#include "test.h"

#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

#include <new>
#include <float.h>

#include "goodness_test.h"
#include "one_mean_t_test.h"
#include "one_proportion_z_test.h"
#include "slope_t_test.h"
#include "two_means_t_test.h"
#include "two_proportions_z_test.h"

namespace Inference {

void Test::setGraphTitle(char * buffer, size_t bufferSize) const {
  const char * format = I18n::translate(graphTitleFormat());
  Poincare::Print::customPrintf(buffer, bufferSize, format, testCriticalValue(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits, pValue(), Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
}

bool Test::initializeSignificanceTest(SignificanceTestType testType, Shared::GlobalContext * context) {
  if (!Statistic::initializeSignificanceTest(testType, context)) {
    return false;
  }
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
    case SignificanceTestType::Slope:
      new (this) SlopeTTest(context);
      break;
    default:
      assert(testType == SignificanceTestType::Categorical);
      new (this) GoodnessTest();
      break;
  }
  initParameters();
  return true;
}

bool Test::canRejectNull() {
  assert(m_threshold >= 0 && m_threshold <= 1);
  return pValue() <= m_threshold;
}

double Test::thresholdAbscissa(HypothesisParams::ComparisonOperator op) const {
  assert(op != HypothesisParams::ComparisonOperator::Different);
  return cumulativeDistributiveInverseForProbability(op == HypothesisParams::ComparisonOperator::Lower ? threshold() : 1.0 - threshold());
}

void Test::resultAtIndex(int index, double * value, Poincare::Layout * message, I18n::Message * subMessage, int * precision) {
  if (index < numberOfEstimates()) {
    *value = estimateValue(index);
    *message = estimateLayout(index);
    *subMessage = estimateDescription(index);
    return;
  }
  index -= numberOfEstimates();
  switch (index) {
    case ResultOrder::Z:
      *value = testCriticalValue();
      *message = testCriticalValueSymbol();
      *subMessage = I18n::Message::TestStatistic;
      break;
    case ResultOrder::PValue:
      *value = pValue();
      *message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::PValue));
      *subMessage = I18n::Message::Default;
      break;
    default:
      assert(index == ResultOrder::TestDegree);
      *value = degreeOfFreedom();
      *message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::DegreesOfFreedom));
      *subMessage = I18n::Message::Default;
      /* We reduce the precision since "Degrees of freedom" might not fit in
       * all languages with 7 significant digits. */
      *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
  }
}

static float interpolate(float a, float b, float alpha) {
  return alpha * (b-a) + a;
}

void Test::computeCurveViewRange(float transition) {
  // Transition goes from 0 (default view) to 1 (zoomed view)
  float alpha = cumulativeDistributiveInverseForProbability(1 - threshold());
  float z = testCriticalValue();
  float margin = std::abs(alpha - z) * k_displayZoomedInHorizontalMarginRatio;
  if (alpha == z) {
    // Arbitrary value to provide some zoom if we can't separate α and z
    margin = 0.1;
  }
  float targetXMin = std::min(alpha, z) - margin;
  float targetXMax = std::max(alpha, z) + margin;
  float targetXCenter = (alpha + z) / 2;
  float targetYMax = std::max(evaluateAtAbscissa(alpha), evaluateAtAbscissa(z)) * (1 + k_displayZoomedInTopMarginRatio);
  float cMin = computeXMin();
  float cMax = computeXMax();
  // We want each zoom step to scale the width by the same factor
  float width = std::exp(interpolate(std::log(cMax-cMin), std::log(targetXMax-targetXMin), transition));
  // and the middle of target range to progress linearly to the screen center
  float ratio = interpolate((targetXCenter-cMin)/(cMax-cMin), 0.5, transition);
  float xMin = targetXCenter - ratio * width ;
  float xMax = xMin + width;

  float height = std::exp(interpolate(std::log(computeYMax()), std::log(targetYMax), transition));
  float yMax = height;
  float yMin =  -k_displayBottomMarginRatio * height;
  protectedSetXMin(xMin, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetXMax(xMax, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);
  protectedSetYMin(yMin, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, false);
  protectedSetYMax(yMax, Shared::Range1D::k_lowerMaxFloat, Shared::Range1D::k_upperMaxFloat, true);

}

}  // namespace Inference
