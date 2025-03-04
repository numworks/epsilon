#pragma once

#include <apps/i18n.h>
#include <omg/unreachable.h>
#include <poincare/statistics/inference.h>

namespace Inference {

enum class Message {
  Title,
  BasicTitle,
  TStatisticMessage,
  ZStatisticMessage,
  TOrZStatisticMessage,
  TDistributionName,
  TPooledDistributionName,
  ZDistributionName,
  ThresholdName,
  ThresholdDescription,
};

constexpr I18n::Message GetMessage(Poincare::Inference::Method method,
                                   Message message) {
  // Same order as in the enum Message
  constexpr I18n::Message testMessages[] = {
      I18n::Message::TestDescr,   I18n::Message::Test,
      I18n::Message::TTest,       I18n::Message::ZTest,
      I18n::Message::TOrZTest,    I18n::Message::TTest,
      I18n::Message::PooledTTest, I18n::Message::ZTest,
      I18n::Message::GreekAlpha,  I18n::Message::SignificanceLevel,
  };
  constexpr I18n::Message intervalMessages[] = {
      I18n::Message::IntervalDescr,      I18n::Message::Interval,
      I18n::Message::TInterval,          I18n::Message::ZInterval,
      I18n::Message::TOrZInterval,       I18n::Message::TInterval,
      I18n::Message::PooledTInterval,    I18n::Message::ZInterval,
      I18n::Message::ConfidenceInterval, I18n::Message::Default,
  };
  int index = static_cast<int>(message);
  return method == Poincare::Inference::Method::SignificanceTest
             ? testMessages[index]
             : intervalMessages[index];
}

constexpr I18n::Message DistributionTitle(
    Poincare::Inference::TestType testType) {
  if (testType == Poincare::Inference::TestType::OneMean) {
    return I18n::Message::TypeControllerTitleOne;
  }
  if (testType == Poincare::Inference::TestType::TwoMeans) {
    return I18n::Message::TypeControllerTitleTwo;
  }
  return I18n::Message::Default;
}

constexpr I18n::Message Title(Poincare::Inference::Type type) {
  switch (type.testType) {
    case Poincare::Inference::TestType::OneMean:
      return type.statisticType == Poincare::Inference::StatisticType::T
                 ? I18n::Message::HypothesisControllerTitleOneMeanT
                 : I18n::Message::HypothesisControllerTitleOneMeanZ;
    case Poincare::Inference::TestType::TwoMeans:
      switch (type.statisticType) {
        case Poincare::Inference::StatisticType::T:
          return I18n::Message::HypothesisControllerTitleTwoMeansT;
        case Poincare::Inference::StatisticType::TPooled:
          return I18n::Message::HypothesisControllerTitleTwoMeansPooledT;
        case Poincare::Inference::StatisticType::Z:
          return I18n::Message::HypothesisControllerTitleTwoMeansZ;
        default:
          OMG::unreachable();
      }
    case Poincare::Inference::TestType::OneProportion:
      return I18n::Message::HypothesisControllerTitleOneProp;
    case Poincare::Inference::TestType::TwoProportions:
      return I18n::Message::HypothesisControllerTitleTwoProps;
    case Poincare::Inference::TestType::Slope:
      return I18n::Message::HypothesisControllerTitleSlope;
    case Poincare::Inference::TestType::Chi2:
      switch (type.categoricalType) {
        case Poincare::Inference::CategoricalType::GoodnessOfFit:
          return I18n::Message::InputGoodnessControllerTitle;
        case Poincare::Inference::CategoricalType::Homogeneity:
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
  bool isZ = type.statisticType == Poincare::Inference::StatisticType::Z;
  switch (type.testType) {
    case Poincare::Inference::TestType::OneMean:
      switch (index) {
        case Poincare::Inference::Params::OneMean::X:
          return I18n::Message::SampleMean;
        case Poincare::Inference::Params::OneMean::S:
          return isZ ? I18n::Message::PopulationStd : I18n::Message::SampleSTD;
        case Poincare::Inference::Params::OneMean::N:
          return I18n::Message::SampleSize;
        default:
          OMG::unreachable();
      }
    case Poincare::Inference::TestType::TwoMeans:
      switch (index) {
        case Poincare::Inference::Params::TwoMeans::X1:
          return I18n::Message::Sample1Mean;
        case Poincare::Inference::Params::TwoMeans::S1:
          return isZ ? I18n::Message::Population1Std
                     : I18n::Message::Sample1Std;
        case Poincare::Inference::Params::TwoMeans::N1:
          return I18n::Message::Sample1Size;
        case Poincare::Inference::Params::TwoMeans::X2:
          return I18n::Message::Sample2Mean;
        case Poincare::Inference::Params::TwoMeans::S2:
          return isZ ? I18n::Message::Population2Std
                     : I18n::Message::Sample2Std;
        case Poincare::Inference::Params::TwoMeans::N2:
          return I18n::Message::Sample2Size;
        default:
          OMG::unreachable();
      }
    case Poincare::Inference::TestType::OneProportion:
      switch (index) {
        case Poincare::Inference::Params::OneProportion::X:
          return I18n::Message::NumberOfSuccesses;
        case Poincare::Inference::Params::OneProportion::N:
          return I18n::Message::SampleSize;
        default:
          OMG::unreachable();
      }
    case Poincare::Inference::TestType::TwoProportions:
      switch (index) {
        case Poincare::Inference::Params::TwoProportions::X1:
          return I18n::Message::SuccessSample1;
        case Poincare::Inference::Params::TwoProportions::N1:
          return I18n::Message::Sample1Size;
        case Poincare::Inference::Params::TwoProportions::X2:
          return I18n::Message::SuccessSample2;
        case Poincare::Inference::Params::TwoProportions::N2:
          return I18n::Message::Sample2Size;
        default:
          OMG::unreachable();
      }
    case Poincare::Inference::TestType::Slope:
      switch (index) {
        case Poincare::Inference::Params::Slope::N:
          return I18n::Message::SampleSize;
        case Poincare::Inference::Params::Slope::B:
          return I18n::Message::SampleSlope;
        case Poincare::Inference::Params::Slope::SE:
          return I18n::Message::StandardError;
        default:
          OMG::unreachable();
      }
    default:
      return I18n::Message::Default;
  }
}

constexpr I18n::Message IntervalEstimateDescription(
    Poincare::Inference::TestType testType) {
  switch (testType) {
    case Poincare::Inference::TestType::TwoMeans:
      return I18n::Message::SampleTwoMeans;
    case Poincare::Inference::TestType::OneProportion:
      return I18n::Message::SampleProportion;
    case Poincare::Inference::TestType::TwoProportions:
      return I18n::Message::SampleTwoProportions;
    default:
      return I18n::Message::Default;
  }
}

constexpr I18n::Message TestEstimateDescription(
    Poincare::Inference::TestType testType, int index) {
  switch (testType) {
    case Poincare::Inference::TestType::OneProportion:
      return IntervalEstimateDescription(testType);
    case Poincare::Inference::TestType::TwoProportions:
      switch (index) {
        case Poincare::Inference::EstimatesOrder::TwoProportions::P1:
          return I18n::Message::Sample1Proportion;
        case Poincare::Inference::EstimatesOrder::TwoProportions::P2:
          return I18n::Message::Sample2Proportion;
        case Poincare::Inference::EstimatesOrder::TwoProportions::Pooled:
          return I18n::Message::PooledProportion;
        default:
          OMG::unreachable();
      }
    default:
      return I18n::Message::Default;
  }
}

}  // namespace Inference
