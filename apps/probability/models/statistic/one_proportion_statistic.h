#ifndef PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneProportionStatistic : public ZStatistic {
public:
  void init(Data::SubApp subapp) override;

  bool isValidH0(float h0) override { return h0 >= 0 && h0 <= 1; }
  bool isValidParamAtIndex(int i, float p) override;
  void setParamAtIndex(int index, float p) override;

  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "p̂"; };
  Poincare::Layout estimateLayout() override;
  I18n::Message estimateDescription() override { return I18n::Message::SampleProportion; };

protected:
  int numberOfStatisticParameters() const override { return k_numberOfParams; }

private:
  // Parameters
  constexpr static int k_numberOfParams = 2;
  enum ParamsOrder { X, N };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  float * paramArray() override { return m_params; }
  float m_params[k_numberOfParams];
  float x() { return m_params[ParamsOrder::X]; }
  float n() { return m_params[ParamsOrder::N]; }

  bool validateInputs() override;

  // Computation
  float computeEstimate(float x, float n);
  float computeZ(float p0, float p, float n);
  float computeStandardError(float pEstimate, float n);
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H */
