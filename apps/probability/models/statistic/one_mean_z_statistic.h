#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneMeanZStatistic : public ZStatistic {
public:
  OneMeanZStatistic();
  bool isValidParamAtIndex(int i, float p) override;
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x̅"; };

protected:
  // Parameters
  constexpr static int k_numberOfParams = 3;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X, N, Sigma };
  ParameterRepr paramReprAtIndex(int i) const override;
  float * paramArray() override { return m_params; }

private:
  float x() { return m_params[ParamsOrder::X]; }
  float n() { return m_params[ParamsOrder::N]; }
  float sigma() { return m_params[ParamsOrder::Sigma]; }

  // Computation
  float computeZ(float meanSample, float mean, float n, float sigma);
  float computeStandardError(float sigma, float n);

  float m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H */
