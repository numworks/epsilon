#ifndef INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H

#include "interval.h"
#include "test.h"

namespace Inference {

class TwoProportionsStatistic {
 protected:
  double preProcessTwoPropsParameter(double p, int index) const {
    if (index == PcrInference::Params::TwoProportions::N1 ||
        index == PcrInference::Params::TwoProportions::X1 ||
        index == PcrInference::Params::TwoProportions::N2 ||
        index == PcrInference::Params::TwoProportions::X2) {
      return std::round(p);
    }
    return p;
  }

  double m_params[PcrInference::NumberOfParameters(
      PcrInference::TestType::TwoProportions)];
};

class TwoProportionsZInterval : public Interval,
                                public TwoProportionsStatistic {
 public:
  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::TwoProportions;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessTwoPropsParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

class TwoProportionsZTest : public Test, public TwoProportionsStatistic {
 public:
  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::TwoProportions;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessTwoPropsParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

}  // namespace Inference

#endif
