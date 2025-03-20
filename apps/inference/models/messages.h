#pragma once

#include <apps/i18n.h>
#include <omg/unreachable.h>
#include <poincare/statistics/inference.h>

#include "aliases.h"

namespace Inference {

enum class Message {
  SubAppTitle,
  SubAppBasicTitle,
  TStatisticMessage,
  ZStatisticMessage,
  TOrZStatisticMessage,
  TDistributionName,
  TPooledDistributionName,
  ZDistributionName,
  ThresholdName,
  ThresholdDescription,
};

constexpr I18n::Message GetMessage(SubApp subApp, Message message) {
  // Same order as in the enum Message
  constexpr I18n::Message testMessages[] = {
      I18n::Message::TestDescr,   I18n::Message::Test,
      I18n::Message::TTest,       I18n::Message::ZTest,
      I18n::Message::TOrZTest,    I18n::Message::TTest,
      I18n::Message::PooledTTest, I18n::Message::ZTest,
      I18n::Message::GreekAlpha,  I18n::Message::SignificanceLevel,
  };
  constexpr I18n::Message intervalMessages[] = {
      I18n::Message::IntervalDescr,   I18n::Message::Interval,
      I18n::Message::TInterval,       I18n::Message::ZInterval,
      I18n::Message::TOrZInterval,    I18n::Message::TInterval,
      I18n::Message::PooledTInterval, I18n::Message::ZInterval,
      I18n::Message::ConfidenceLevel, I18n::Message::Default,
  };
  int index = static_cast<int>(message);
  return subApp == SubApp::SignificanceTest ? testMessages[index]
                                            : intervalMessages[index];
}

constexpr I18n::Message DistributionTitle(TestType testType) {
  if (testType == TestType::OneMean) {
    return I18n::Message::TypeControllerTitleOne;
  }
  if (testType == TestType::TwoMeans) {
    return I18n::Message::TypeControllerTitleTwo;
  }
  return I18n::Message::Default;
}

constexpr I18n::Message Title(Poincare::Inference::Type type) {
  switch (type.testType) {
    case TestType::OneMean:
      return type.statisticType == StatisticType::T
                 ? I18n::Message::HypothesisControllerTitleOneMeanT
                 : I18n::Message::HypothesisControllerTitleOneMeanZ;
    case TestType::TwoMeans:
      switch (type.statisticType) {
        case StatisticType::T:
          return I18n::Message::HypothesisControllerTitleTwoMeansT;
        case StatisticType::TPooled:
          return I18n::Message::HypothesisControllerTitleTwoMeansPooledT;
        case StatisticType::Z:
          return I18n::Message::HypothesisControllerTitleTwoMeansZ;
        default:
          OMG::unreachable();
      }
    case TestType::OneProportion:
      return I18n::Message::HypothesisControllerTitleOneProp;
    case TestType::TwoProportions:
      return I18n::Message::HypothesisControllerTitleTwoProps;
    case TestType::Slope:
      return I18n::Message::HypothesisControllerTitleSlope;
    case TestType::Chi2:
      switch (type.categoricalType) {
        case CategoricalType::GoodnessOfFit:
          return I18n::Message::InputGoodnessControllerTitle;
        case CategoricalType::Homogeneity:
          return I18n::Message::InputHomogeneityControllerTitle;
        default:
          OMG::unreachable();
      }
    default:
      OMG::unreachable();
  }
}

constexpr I18n::Message ParameterDescriptionAtIndex(
    Poincare::Inference::Type type, int index) {
  bool isZ = type.statisticType == StatisticType::Z;
  switch (type.testType) {
    case TestType::OneMean:
      switch (index) {
        case Params::OneMean::X:
          return I18n::Message::SampleMean;
        case Params::OneMean::S:
          return isZ ? I18n::Message::PopulationStd : I18n::Message::SampleSTD;
        case Params::OneMean::N:
          return I18n::Message::SampleSize;
        default:
          OMG::unreachable();
      }
    case TestType::TwoMeans:
      switch (index) {
        case Params::TwoMeans::X1:
          return I18n::Message::Sample1Mean;
        case Params::TwoMeans::S1:
          return isZ ? I18n::Message::Population1Std
                     : I18n::Message::Sample1Std;
        case Params::TwoMeans::N1:
          return I18n::Message::Sample1Size;
        case Params::TwoMeans::X2:
          return I18n::Message::Sample2Mean;
        case Params::TwoMeans::S2:
          return isZ ? I18n::Message::Population2Std
                     : I18n::Message::Sample2Std;
        case Params::TwoMeans::N2:
          return I18n::Message::Sample2Size;
        default:
          OMG::unreachable();
      }
    case TestType::OneProportion:
      switch (index) {
        case Params::OneProportion::X:
          return I18n::Message::NumberOfSuccesses;
        case Params::OneProportion::N:
          return I18n::Message::SampleSize;
        default:
          OMG::unreachable();
      }
    case TestType::TwoProportions:
      switch (index) {
        case Params::TwoProportions::X1:
          return I18n::Message::SuccessSample1;
        case Params::TwoProportions::N1:
          return I18n::Message::Sample1Size;
        case Params::TwoProportions::X2:
          return I18n::Message::SuccessSample2;
        case Params::TwoProportions::N2:
          return I18n::Message::Sample2Size;
        default:
          OMG::unreachable();
      }
    case TestType::Slope:
      switch (index) {
        case Params::Slope::N:
          return I18n::Message::SampleSize;
        case Params::Slope::B:
          return I18n::Message::SampleSlope;
        case Params::Slope::SE:
          return I18n::Message::StandardError;
        default:
          OMG::unreachable();
      }
    default:
      return I18n::Message::Default;
  }
}

constexpr I18n::Message IntervalEstimateDescription(TestType testType) {
  switch (testType) {
    case TestType::TwoMeans:
      return I18n::Message::SampleTwoMeans;
    case TestType::OneProportion:
      return I18n::Message::SampleProportion;
    case TestType::TwoProportions:
      return I18n::Message::SampleTwoProportions;
    default:
      return I18n::Message::Default;
  }
}

constexpr I18n::Message TestEstimateDescription(TestType testType, int index) {
  switch (testType) {
    case TestType::OneProportion:
      return IntervalEstimateDescription(testType);
    case TestType::TwoProportions:
      switch (index) {
        case Poincare::Inference::SignificanceTest::EstimatesOrder::
            TwoProportions::P1:
          return I18n::Message::Sample1Proportion;
        case Poincare::Inference::SignificanceTest::EstimatesOrder::
            TwoProportions::P2:
          return I18n::Message::Sample2Proportion;
        case Poincare::Inference::SignificanceTest::EstimatesOrder::
            TwoProportions::Pooled:
          return I18n::Message::PooledProportion;
        default:
          OMG::unreachable();
      }
    default:
      return I18n::Message::Default;
  }
}

}  // namespace Inference
