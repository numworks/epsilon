#ifndef PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class TwoProportionsStatistic : public ZStatistic {
public:
  void tidy() override;
  void init(Data::SubApp subapp) override;
  bool isValidH0(double p) override;
  bool isValidParamAtIndex(int i, double p) override;
  void setParamAtIndex(int index, double p) override;

  void computeTest() override;
  void computeInterval() override;

  // Test statistic
  bool hasDegreeOfFreedom() override { return false; };

  // Confidence interval
  const char * estimateSymbol() override { return "p̂1-p̂2"; };
  Poincare::Layout estimateLayout() override;
  I18n::Message estimateDescription() override { return I18n::Message::SampleTwoProportions; };

protected:
  // Parameters
  constexpr static int k_numberOfParams = 4;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X1, N1, X2, N2 };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  double * paramArray() override { return m_params; }

  bool validateInputs() override;

private:
  double x1() { return m_params[ParamsOrder::X1]; }
  double n1() { return m_params[ParamsOrder::N1]; }
  double x2() { return m_params[ParamsOrder::X2]; }
  double n2() { return m_params[ParamsOrder::N2]; }

  // Computation
  double computeEstimate(double x1, double n1, double x2, double n2);
  double computeZ(double deltaP0, double x1, int n1, double x2, int n2);
  double computeStandardError(double p1Estimate, int n1, double p2Estimate, int n2);

  // TODO: Factorize in IntervalStatistics
  Poincare::Layout m_estimateLayout;
  double m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H */
