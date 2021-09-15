#ifndef PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H

#include "t_statistic.h"

namespace Probability {

// TODO factor with OneMeanZStatistic
class OneMeanTStatistic : public TStatistic {
public:
  void init(Data::SubApp subapp) override;
  bool isValidParamAtIndex(int i, float p) override;
  void setParamAtIndex(int index, float p) override;

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
  float * paramArray() override { return m_params; }

private:
  float x() { return m_params[ParamsOrder::X]; }
  float s() { return m_params[ParamsOrder::S]; }
  float n() { return m_params[ParamsOrder::N]; }

  // Computation
  float computeDegreesOfFreedom(float n);
  float computeT(float mean, float meanSample, float s, float n);
  float computeStandardError(float s, float n);

  float m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H */
