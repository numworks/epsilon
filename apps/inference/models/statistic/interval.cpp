#include "interval.h"
#include <assert.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/print.h>
#include <poincare/vertical_offset_layout.h>

#include <new>
#include <float.h>

#include "one_mean_t_interval.h"
#include "one_proportion_z_interval.h"
#include "slope_t_interval.h"
#include "two_means_t_interval.h"
#include "two_proportions_z_interval.h"

namespace Inference {

Interval::~Interval() {
  tidy();
}

void Interval::setGraphTitleForValue(double marginOfError, char * buffer, size_t bufferSize) const {
  const char * format = I18n::translate(I18n::Message::StatisticGraphControllerIntervalTitleFormat);
  Poincare::Print::CustomPrintf(buffer, bufferSize, format, marginOfError, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
}

void Interval::setResultTitleForValues(double estimate, double threshold, char * buffer, size_t bufferSize, bool resultIsTopPage) const {
  const char * confidence = I18n::translate(I18n::Message::Confidence);
  if (resultIsTopPage) {
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%s=%*.*ed %s=%*.*ed",
        estimateSymbol(),
        estimate, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits,
        confidence,
        threshold, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  } else {
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%s=%*.*ed",
        confidence,
        threshold, Poincare::Preferences::PrintFloatMode::Decimal, Poincare::Preferences::ShortNumberOfSignificantDigits);
  }
}

bool Interval::initializeSignificanceTest(SignificanceTestType testType, Shared::GlobalContext * context) {
  if (!Statistic::initializeSignificanceTest(testType, context)) {
    return false;
  }
  this->~Interval();
  switch (testType) {
    case SignificanceTestType::OneMean:
      new (this) OneMeanTInterval();
      break;
    case SignificanceTestType::TwoMeans:
      new (this) TwoMeansTInterval();
      break;
    case SignificanceTestType::OneProportion:
      new (this) OneProportionZInterval();
      break;
    case SignificanceTestType::TwoProportions:
      new (this) TwoProportionsZInterval();
      break;
    default:
      assert(testType == SignificanceTestType::Slope);
      new (this) SlopeTInterval(context);
      break;
  }
  initParameters();
  return true;
}

void Interval::tidy() {
  m_estimateLayout = Poincare::Layout();
}

Poincare::Layout Interval::intervalCriticalValueSymbol() {
  return Poincare::HorizontalLayout::Builder(
      testCriticalValueSymbol(),
      Poincare::VerticalOffsetLayout::Builder(
          Poincare::CodePointLayout::Builder('*'),
          Poincare::VerticalOffsetLayoutNode::VerticalPosition::Superscript));
}


bool Interval::isGraphable() const {
  double SE = standardError();
  assert(std::isnan(SE) || SE >= 0);
  return !std::isnan(SE) && SE >= FLT_MIN;
}

float Interval::computeXMin() const {
  return estimate() - const_cast<Interval *>(this)->largestMarginOfError() * k_intervalMarginRatio;
}

float Interval::computeXMax() const {
  return estimate() + const_cast<Interval *>(this)->largestMarginOfError() * k_intervalMarginRatio;
}

double Interval::computeIntervalCriticalValue() {
  double value = 0.5 + m_threshold / 2;
  return cumulativeDistributiveInverseForProbability(value);
}

float Interval::largestMarginOfError() {
  /* Temporarily sets the statistic's threshold to the largest displayed
   * interval to compute the margin of error needed to display all intervals. */
  double previousThreshold = threshold();
  float intervalTemp = DisplayedIntervalThresholdAtIndex(previousThreshold, k_numberOfDisplayedIntervals - 1);
  m_threshold = intervalTemp;
  compute();
  double error = marginOfError();
  // Restore the statistic
  setThreshold(previousThreshold);
  compute();
  return error;
}

float Interval::DisplayedIntervalThresholdAtIndex(float mainThreshold, int index) {
  int direction = index - MainDisplayedIntervalThresholdIndex(mainThreshold);
  if (direction == 0) {
    return mainThreshold;
  }
  // There are significant interval thresholds to display
  constexpr size_t k_numberOfSignificantThresholds = 14;
  constexpr float k_significantThresholds[k_numberOfSignificantThresholds] = {
      0.1f, 0.2f,  0.3f, 0.4f,  0.5f,  0.6f,  0.7f,
      0.8f, 0.85f, 0.9f, 0.95f, 0.98f, 0.99f, 0.999f};
  assert((direction > 0 && mainThreshold <= 0.99f) || (direction < 0 && mainThreshold > 0.1f));
  // If threshold is in ]0.99, 1.0] display 0.99 with a direction of 1
  int significantThresholdIndex = k_numberOfSignificantThresholds - 1;
  // If threshold is in ]0.2, 0.3], display 0.4 with a direction of 1, 0.2 if -1
  for (size_t i = 0; i < k_numberOfSignificantThresholds; i++) {
    if (mainThreshold <= k_significantThresholds[i]) {
      significantThresholdIndex = i;
      break;
    }
  }
  size_t nextIndex = significantThresholdIndex + direction;
  assert(nextIndex >= 0 && nextIndex < k_numberOfSignificantThresholds);
  return k_significantThresholds[nextIndex];
}

int Interval::MainDisplayedIntervalThresholdIndex(float mainThreshold) {
  constexpr float k_thresholdLimits[k_numberOfDisplayedIntervals - 1] = {0.1f, 0.2f, 0.99f};
  // If mainThreshold is in ]0.2, 0.99], it is the third displayed interval
  for (int index = 0; index < k_numberOfDisplayedIntervals - 1; index++) {
    if (mainThreshold <= k_thresholdLimits[index]) {
      return index;
    }
  }
  return k_numberOfDisplayedIntervals - 1;
}

void Interval::resultAtIndex(int index, double * value, Poincare::Layout * message, I18n::Message * subMessage, int * precision) {
  // Estimate cell is not displayed -> shift i
  index += estimateLayout().isUninitialized();
  switch (index) {
    case ResultOrder::Estimate:
      *value = estimate();
      *message = estimateLayout();
      *subMessage = estimateDescription();
      break;
    case ResultOrder::Critical:
      *value = intervalCriticalValue();
      *message = intervalCriticalValueSymbol();
      *subMessage = I18n::Message::CriticalValue;
      break;
    case ResultOrder::SE:
      *value = standardError();
      *message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::SE));
      *subMessage = I18n::Message::StandardError;
      break;
    case ResultOrder::ME:
      *value = marginOfError();
      *message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::ME));
      *subMessage = I18n::Message::MarginOfError;
      break;
    default:
      assert(index == ResultOrder::IntervalDegree);
      *value = degreeOfFreedom();
      *message = Poincare::LayoutHelper::String(I18n::translate(I18n::Message::DegreesOfFreedom));
      *subMessage = I18n::Message::Default;
      /* We reduce the precision since "Degrees of freedom" might not fit in
       * all languages with 7 significant digits. */
      *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
      break;
  }
}

}
