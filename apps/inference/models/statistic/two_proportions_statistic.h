#ifndef INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H

#include "interval.h"
#include "test.h"

namespace Inference {

class TwoProportionsStatistic {
 protected:
  double preProcessTwoPropsParameter(double p, int index) const {
    if (index == Params::TwoProportions::N1 ||
        index == Params::TwoProportions::X1 ||
        index == Params::TwoProportions::N2 ||
        index == Params::TwoProportions::X2) {
      return std::round(p);
    }
    return p;
  }

  double m_params[Poincare::Inference::NumberOfParameters(
      TestType::TwoProportions)];
};

class TwoProportionsZInterval : public Interval,
                                public TwoProportionsStatistic {
 public:
  constexpr TestType testType() const override {
    return TestType::TwoProportions;
  }
  constexpr StatisticType statisticType() const override {
    return StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessTwoPropsParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

class TwoProportionsZTest : public Test, public TwoProportionsStatistic {
 public:
  constexpr TestType testType() const override {
    return TestType::TwoProportions;
  }
  constexpr StatisticType statisticType() const override {
    return StatisticType::Z;
  }

  double preProcessParameter(double p, int index) const override {
    return preProcessTwoPropsParameter(p, index);
  }

 private:
  double* parametersArray() override { return m_params; }
};

}  // namespace Inference

#endif
