#include "test.h"

#include <apps/apps_container_helper.h>
#include <assert.h>
#include <float.h>
#include <inference/app.h>
#include <poincare/layout.h>
#include <poincare/print.h>

#include <algorithm>
#include <new>

#include "goodness_test.h"
#include "one_mean_test.h"
#include "one_proportion_z_test.h"
#include "slope_t_test.h"
#include "two_means_test.h"
#include "two_proportions_z_test.h"

namespace Inference {

void Test::setGraphTitle(char* buffer, size_t bufferSize) const {
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed %s=%*.*ed", criticalValueSymbol(),
      testCriticalValue(), Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      I18n::translate(I18n::Message::PValue), pValue(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits);
}

bool Test::initializeSignificanceTest(SignificanceTestType testType,
                                      Shared::GlobalContext* context) {
  if (!Statistic::initializeSignificanceTest(testType, context)) {
    return false;
  }
  this->~Test();
  switch (testType) {
    case SignificanceTestType::OneMean:
      new (this)
          OneMeanTTest(AppsContainerHelper::sharedAppsContainerGlobalContext());
      break;
    case SignificanceTestType::TwoMeans:
      new (this) TwoMeansTTest(
          AppsContainerHelper::sharedAppsContainerGlobalContext());
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

double Test::thresholdAbscissa(Poincare::ComparisonNode::OperatorType op,
                               double factor) const {
  assert(op != Poincare::ComparisonNode::OperatorType::NotEqual);
  double t = factor * threshold();
  return cumulativeDistributiveInverseForProbability(
      op == Poincare::ComparisonNode::OperatorType::Inferior ? t : 1.0 - t);
}

void Test::resultAtIndex(int index, double* value, Poincare::Layout* message,
                         I18n::Message* subMessage, int* precision) {
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
      *message = criticalValueSymbolLayout();
      *subMessage = I18n::Message::TestStatistic;
      break;
    case ResultOrder::PValue:
      *value = pValue();
      *message =
          Poincare::Layout::String(I18n::translate(I18n::Message::PValue));
      *subMessage = I18n::Message::Default;
      break;
    default:
      assert(index == ResultOrder::TestDegree);
      *value = degreeOfFreedom();
      *message = Poincare::Layout::String(
          I18n::translate(I18n::Message::DegreesOfFreedom));
      *subMessage = I18n::Message::Default;
      /* We reduce the precision since "Degrees of freedom" might not fit in
       * all languages with 7 significant digits. */
      *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
  }
}

static float interpolate(float a, float b, float alpha) {
  return alpha * (b - a) + a;
}

bool Test::hasTwoSides() {
  return hypothesisParams()->comparisonOperator() ==
         Poincare::ComparisonNode::OperatorType::NotEqual;
}

bool Test::computeCurveViewRange(float transition, bool zoomSide) {
  // Transition goes from 0 (default view) to 1 (zoomed view)
  float alpha;
  float z = testCriticalValue();
  if (hypothesisParams()->comparisonOperator() ==
      Poincare::ComparisonNode::OperatorType::NotEqual) {
    if (zoomSide) {
      alpha = thresholdAbscissa(
          Poincare::ComparisonNode::OperatorType::Superior, 0.5);
      z = std::abs(z);
    } else {
      alpha = thresholdAbscissa(
          Poincare::ComparisonNode::OperatorType::Inferior, 0.5);
      z = -std::abs(z);
    }
  } else {
    alpha = thresholdAbscissa(hypothesisParams()->comparisonOperator());
  }
  float margin = std::abs(alpha - z) * k_displayZoomedInHorizontalMarginRatio;
  if (std::abs(alpha) > k_displayWidthToSTDRatio ||
      std::abs(z) > k_displayWidthToSTDRatio || alpha * z < 0) {
    // Alpha or z is out of the view or their signs differ, don't try to zoom
    Shared::Inference::computeCurveViewRange();
    return false;
  }
  if (alpha == z) {
    // Arbitrary value to provide some zoom if we can't separate α and z
    margin = 0.1;
  }
  float targetXMin = std::min(alpha, z) - margin;
  float targetXMax = std::max(alpha, z) + margin;
  float targetXCenter = (alpha + z) / 2;
  float targetYMax =
      std::max(evaluateAtAbscissa(alpha), evaluateAtAbscissa(z)) *
      (1 + k_displayZoomedInTopMarginRatio);
  assert(targetYMax >= 0.f);
  if (targetYMax == 0.f) {
    // Arbitrary value to provide some zoom if targetYMax is null
    targetYMax = 1.0f;
  }
  float cMin = computeXMin();
  float cMax = computeXMax();
  // We want each zoom step to scale the width by the same factor
  float width = std::exp(interpolate(
      std::log(cMax - cMin), std::log(targetXMax - targetXMin), transition));
  // and the middle of target range to progress linearly to the screen center
  float ratio =
      interpolate((targetXCenter - cMin) / (cMax - cMin), 0.5, transition);
  float xMin = targetXCenter - ratio * width;
  float xMax = xMin + width;

  float height = std::exp(
      interpolate(std::log(computeYMax()), std::log(targetYMax), transition));
  float yMax = height;
  float yMin = -k_displayBottomMarginRatio * height;
  if (std::isnan(xMin)) {
    xMin = -FLT_MAX;
  }
  if (std::isnan(xMax)) {
    xMax = FLT_MAX;
  }
  assert(std::isfinite(yMin) && std::isfinite(yMax));
  protectedSetXRange(xMin, xMax);
  protectedSetYRange(yMin, yMax);
  return true;
}

const char* Test::criticalValueSymbol() const {
  return distribution()->criticalValueSymbol();
}

}  // namespace Inference
