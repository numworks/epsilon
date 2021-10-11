#ifndef PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneMeanZStatistic : public ZStatistic {
public:
  void init(Data::SubApp subapp) override;

  bool isValidParamAtIndex(int i, double p) override;
  void setParamAtIndex(int index, double p) override;

  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x̅"; };

protected:
  // Parameters
  constexpr static int k_numberOfParams = 3;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X, N, Sigma };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  double * paramArray() override { return m_params; }

private:
  double x() { return m_params[ParamsOrder::X]; }
  double n() { return m_params[ParamsOrder::N]; }
  double sigma() { return m_params[ParamsOrder::Sigma]; }

  // Computation
  double computeZ(double meanSample, double mean, double n, double sigma);
  double computeStandardError(double sigma, double n);

  double m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H */
