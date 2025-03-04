#include "test.h"

#include <apps/apps_container_helper.h>
#include <assert.h>
#include <float.h>
#include <inference/app.h>
#include <poincare/layout.h>
#include <poincare/print.h>

#include <algorithm>

#include "statistic.h"

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

void Test::initParameters() {
  m_hypothesis = PcrInference::DefaultHyphothesis(testType());
  Statistic::initParameters();
}

bool Test::canRejectNull() {
  assert(m_threshold >= 0 && m_threshold <= 1);
  return pValue() <= m_threshold;
}

double Test::thresholdAbscissa(Poincare::ComparisonJunior::Operator op,
                               double factor) const {
  assert(op != Poincare::ComparisonJunior::Operator::NotEqual);
  double t = factor * threshold();
  return cumulativeDistributiveInverseForProbability(
      op == Poincare::ComparisonJunior::Operator::Inferior ? t : 1.0 - t);
}

void Test::inferenceResultAtIndex(int index, double* value,
                                  Poincare::Layout* message,
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
      *message = criticalValueLayout();
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

void Test::compute() {
  const PcrInference::ParametersArray params = constParametersArray();
  PcrInference::Type type = this->type();
  PcrInference::SignificanceTestResults results =
      PcrInference::ComputeSignificanceTest(type, m_hypothesis, params);
  m_degreesOfFreedom = results.degreesOfFreedom;
  m_testCriticalValue = results.criticalValue;
  m_pValue = results.pValue;
  m_estimates = results.estimates;
}

static float interpolate(float a, float b, float alpha) {
  return alpha * (b - a) + a;
}

bool Test::hasTwoSides() {
  return m_hypothesis.m_alternative ==
         Poincare::ComparisonJunior::Operator::NotEqual;
}

bool Test::shouldForbidZoom(float alpha, float criticalValue) {
  /* This method only applies on standardardized normal distribution,
   * otherwise the method needs to be overridden. */
  // Alpha or criticalValue is out of the view or their signs differ
  return std::abs(alpha) > k_displayWidthToSTDRatio ||
         std::abs(criticalValue) > k_displayWidthToSTDRatio ||
         alpha * criticalValue < 0;
}

bool Test::computeCurveViewRange(float transition, bool zoomSide) {
  // Transition goes from 0 (default view) to 1 (zoomed view)
  float alpha;
  float z = testCriticalValue();
  if (hypothesis()->m_alternative ==
      Poincare::ComparisonJunior::Operator::NotEqual) {
    if (zoomSide) {
      alpha = thresholdAbscissa(Poincare::ComparisonJunior::Operator::Superior,
                                0.5);
      z = std::abs(z);
    } else {
      alpha = thresholdAbscissa(Poincare::ComparisonJunior::Operator::Inferior,
                                0.5);
      z = -std::abs(z);
    }
  } else {
    alpha = thresholdAbscissa(hypothesis()->m_alternative);
  }
  if (shouldForbidZoom(alpha, z)) {
    Shared::Inference::computeCurveViewRange();
    return false;
  }
  float margin = std::abs(alpha - z) * k_displayZoomedInHorizontalMarginRatio;
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

}  // namespace Inference
