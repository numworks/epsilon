#include "inference.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/global_context.h>
#include <inference/app.h>
#include <omg/unreachable.h>
#include <poincare/statistics/inference.h>

#include "goodness_test.h"
#include "homogeneity_test.h"
#include "one_mean_interval.h"
#include "one_mean_test.h"
#include "one_proportion_statistic.h"
#include "slope_t_statistic.h"
#include "two_means_interval.h"
#include "two_means_test.h"
#include "two_proportions_statistic.h"

namespace Inference {

static Shared::GlobalContext* getContext() {
  return AppsContainerHelper::sharedAppsContainerGlobalContext();
}

static void* initialize(SubApp subApp, Poincare::Inference::Type type,
                        Inference* target) {
  switch (subApp) {
    case SubApp::SignificanceTest:
      switch (type.testType) {
        case TestType::OneMean:
          switch (type.statisticType) {
            case StatisticType::T:
              return new (target) OneMeanTTest(getContext());
            case StatisticType::Z:
              return new (target) OneMeanZTest(getContext());
            default:
              OMG::unreachable();
          }
        case TestType::TwoMeans:
          switch (type.statisticType) {
            case StatisticType::T:
              return new (target) TwoMeansTTest(getContext());
            case StatisticType::TPooled:
              return new (target) PooledTwoMeansTTest(getContext());
            case StatisticType::Z:
              return new (target) TwoMeansZTest(getContext());
            default:
              OMG::unreachable();
          }
        case TestType::OneProportion:
          return new (target) OneProportionZTest();
        case TestType::TwoProportions:
          return new (target) TwoProportionsZTest();
        case TestType::Slope:
          return new (target) SlopeTTest(getContext());
        case TestType::Chi2:
          switch (type.categoricalType) {
            case CategoricalType::GoodnessOfFit:
              return new (target) GoodnessTest();
            case CategoricalType::Homogeneity:
              return new (target) HomogeneityTest();
            default:
              OMG::unreachable();
          }
        default:
          OMG::unreachable();
      }
    case SubApp::ConfidenceInterval:
      switch (type.testType) {
        case TestType::OneMean:
          switch (type.statisticType) {
            case StatisticType::T:
              return new (target) OneMeanTInterval(getContext());
            case StatisticType::Z:
              return new (target) OneMeanZInterval(getContext());
            default:
              OMG::unreachable();
          }
        case TestType::TwoMeans:
          switch (type.statisticType) {
            case StatisticType::T:
              return new (target) TwoMeansTInterval(getContext());
            case StatisticType::TPooled:
              return new (target) PooledTwoMeansTInterval(getContext());
            case StatisticType::Z:
              return new (target) TwoMeansZInterval(getContext());
            default:
              OMG::unreachable();
          }
        case TestType::OneProportion:
          return new (target) OneProportionZInterval();
        case TestType::TwoProportions:
          return new (target) TwoProportionsZInterval();
        case TestType::Slope:
          return new (target) SlopeTInterval(getContext());
        default:
          OMG::unreachable();
      }
  }
}

static bool initializeStatistic(SubApp subApp, Poincare::Inference::Type type,
                                Inference* target) {
  target->~Inference();
  assert(subApp != SubApp::ConfidenceInterval ||
         Poincare::Inference::ConfidenceInterval::
             IsTypeCompatibleWithConfidenceInterval(type));
  assert(Poincare::Inference::IsTestCompatibleWithStatistic(type, type));
  initialize(subApp, type, target);
  target->initParameters();
  return true;
}

bool Inference::initializeSubApp(SubApp subApp) {
  if (subApp == this->subApp()) {
    return false;
  }
  Poincare::Inference::Type dummyType(TestType::OneMean);
  return initializeStatistic(subApp, dummyType, this);
}

bool Inference::initializeTest(TestType testType) {
  if (testType == this->testType()) {
    return false;
  }
  Poincare::Inference::Type partialType(testType);
  return initializeStatistic(subApp(), partialType, this);
}

bool Inference::initializeDistribution(StatisticType statisticType) {
  if (statisticType == this->statisticType()) {
    return false;
  }
  Poincare::Inference::Type type(testType(), statisticType);
  return initializeStatistic(subApp(), type, this);
}

bool Inference::initializeCategoricalType(CategoricalType categoricalType) {
  if (categoricalType == this->categoricalType()) {
    return false;
  }
  assert(testType() == TestType::Chi2);
  Poincare::Inference::Type type(testType(), statisticType(), categoricalType);
  return initializeStatistic(subApp(), type, this);
}

bool Inference::authorizedParameterAtIndex(double p, int i) const {
  if (i == indexOfThreshold()) {
    /* Since p will be converted to float later, we need to ensure that
     * it's not too close to 1.0 */
    return Poincare::Inference::IsThresholdValid(p) &&
           static_cast<float>(p) < 1.0 - OMG::Float::EpsilonLax<float>();
  }
  /* p might be pre-processed when set so we need to check if it's valid after
   * pre-processing */
  p = preProcessParameter(p, i);
  return Shared::StatisticalDistribution::authorizedParameterAtIndex(p, i) &&
         Poincare::Inference::IsParameterValid(type(), p, i);
}

bool Inference::areParametersValid() {
  return Poincare::Inference::AreParametersValid(type(),
                                                 constParametersArray());
}

double Inference::parameterAtIndex(int i) const {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfTestParameters());
  return i == indexOfThreshold()
             ? m_threshold
             : const_cast<Inference*>(this)->parametersArray()[i];
}

void Inference::setParameterAtIndex(double f, int i) {
  assert(i <= indexOfThreshold() &&
         indexOfThreshold() == numberOfTestParameters());
  if (i == indexOfThreshold()) {
    m_threshold = f;
  } else {
    f = preProcessParameter(f, i);
    parametersArray()[i] = f;
  }
}

double Inference::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return Poincare::Distribution::CumulativeDistributiveFunctionAtAbscissa(
      distributionType(), x, distributionParameters());
}

double Inference::cumulativeDistributiveInverseForProbability(
    double probability) const {
  return Poincare::Distribution::CumulativeDistributiveInverseForProbability(
      distributionType(), probability, distributionParameters());
}

int Inference::secondResultSectionStart() const {
  int n = numberOfExtraResults();
  return n == 0 ? -1 : n;
}

void Inference::resultAtIndex(int index, double* value,
                              Poincare::Layout* message,
                              I18n::Message* subMessage, int* precision) {
  if (index < numberOfExtraResults()) {
    extraResultAtIndex(index, value, message, subMessage, precision);
    return;
  }
  index -= numberOfExtraResults();
  inferenceResultAtIndex(index, value, message, subMessage, precision);
}

float Inference::computeYMax() const {
  float max = 0;
  switch (distributionType()) {
    case Poincare::Distribution::Type::Student:
    case Poincare::Distribution::Type::Normal:
      max = canonicalDensityFunction(0);
      break;
    case Poincare::Distribution::Type::Chi2:
      max = m_degreesOfFreedom <= 2.0
                ? 0.5
                : canonicalDensityFunction(m_degreesOfFreedom - 1) * 1.2;
      break;
    default:
      OMG::unreachable();
  }
  return (1 + Shared::StatisticalDistribution::k_displayTopMarginRatio) * max;
}

float Inference::canonicalDensityFunction(float x) const {
  return Poincare::Distribution::EvaluateAtAbscissa<double>(
      distributionType(), x, distributionParameters());
}

}  // namespace Inference
