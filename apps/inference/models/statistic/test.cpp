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
      *precision = Poincare::Preferences::MediumNumberOfSignificantDigits;
  }
}

}  // namespace Inference
