#ifndef PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneProportionStatistic : public ZStatistic {
public:
  void tidy() override;
  void init(Data::SubApp subapp) override;

  bool isValidH0(double h0) override { return h0 > 0 && h0 < 1; }
  bool isValidParamAtIndex(int i, double p) override;
  void setParamAtIndex(int index, double p) override;

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
  double * paramArray() override { return m_params; }
  double m_params[k_numberOfParams];
  double x() { return m_params[ParamsOrder::X]; }
  double n() { return m_params[ParamsOrder::N]; }

  bool validateInputs() override;

  // Computation
  double computeEstimate(double x, double n);
  double computeZ(double p0, double p, double n);
  double computeStandardError(double pEstimate, double n);

  // TODO: Factorize in IntervalStatistics
  Poincare::Layout m_estimateLayout;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H */
