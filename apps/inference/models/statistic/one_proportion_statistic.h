#ifndef INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H
#define INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H

#include "interval.h"
#include "test.h"

namespace Inference {

class OneProportionStatistic {
 protected:
  double preProcessOnePropParameter(double p, int index) const {
    if (index == Params::OneProportion::N ||
        index == Params::OneProportion::X) {
      return std::round(p);
    }
    return p;
  }

  double m_params[Poincare::Inference::NumberOfParameters(
      TestType::OneProportion)];
};

class OneProportionZTest : public Test, public OneProportionStatistic {
 public:
  constexpr TestType testType() const override {
    return TestType::OneProportion;
  }
  constexpr StatisticType statisticType() const override {
    return StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessOnePropParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

class OneProportionZInterval : public Interval, public OneProportionStatistic {
 public:
  constexpr TestType testType() const override {
    return TestType::OneProportion;
  }
  constexpr StatisticType statisticType() const override {
    return StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessOnePropParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

}  // namespace Inference

#endif
