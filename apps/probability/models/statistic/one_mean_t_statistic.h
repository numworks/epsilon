#ifndef PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H

#include "t_statistic.h"

namespace Probability {

// TODO factor with OneMeanZStatistic
class OneMeanTStatistic : public TStatistic {
public:
  void init(Data::SubApp subapp) override;
  bool isValidParamAtIndex(int i, double p) override;
  void setParamAtIndex(int index, double p) override;

  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x̅"; }

protected:
  // Parameters
  constexpr static int k_numberOfParams = 3;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X, S, N };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  double * paramArray() override { return m_params; }

private:
  double x() { return m_params[ParamsOrder::X]; }
  double s() { return m_params[ParamsOrder::S]; }
  double n() { return m_params[ParamsOrder::N]; }

  // Computation
  double computeDegreesOfFreedom(double n);
  double computeT(double mean, double meanSample, double s, double n);
  double computeStandardError(double s, double n);

  double m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H */
