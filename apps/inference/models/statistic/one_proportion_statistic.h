#ifndef INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H
#define INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H

#include "interval.h"
#include "test.h"

namespace Inference {

class OneProportionStatistic {
 protected:
  double preProcessOnePropParameter(double p, int index) const {
    if (index == PcrInference::Params::OneProportion::N ||
        index == PcrInference::Params::OneProportion::X) {
      return std::round(p);
    }
    return p;
  }

  double m_params[PcrInference::NumberOfParameters(
      PcrInference::TestType::OneProportion)];
};

class OneProportionZTest : public Test, public OneProportionStatistic {
 public:
  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::OneProportion;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessOnePropParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

class OneProportionZInterval : public Interval, public OneProportionStatistic {
 public:
  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::OneProportion;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessOnePropParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

}  // namespace Inference

#endif
