#ifndef PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class TwoMeansZStatistic : public ZStatistic {
public:
  // TODO: Factorize in IntervalStatistics
  ~TwoMeansZStatistic();
  void init(Data::SubApp subapp) override;
  bool isValidParamAtIndex(int i, double p) override;
  void setParamAtIndex(int index, double p) override;

  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x̅1-x̅2"; };
  Poincare::Layout estimateLayout() override;
  I18n::Message estimateDescription() override { return I18n::Message::SampleTwoMeans; };


protected:
  // Parameters
  constexpr static int k_numberOfParams = 6;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X1, N1, Sigma1, X2, N2, Sigma2 };
  ParameterRepresentation paramRepresentationAtIndex(int i) const override;
  double * paramArray() override { return m_params; }
  bool validateInputs() override;

private:
  double x1() { return m_params[ParamsOrder::X1]; }
  double n1() { return m_params[ParamsOrder::N1]; }
  double sigma1() { return m_params[ParamsOrder::Sigma1]; }
  double x2() { return m_params[ParamsOrder::X2]; }
  double n2() { return m_params[ParamsOrder::N2]; }
  double sigma2() { return m_params[ParamsOrder::Sigma2]; }


  double _xEstimate(double meanSample1, double meanSample2);
  double computeZ(double deltaMean, double meanSample1, double n1, double sigma1, double meanSample2, double n2,
           double sigma2);
  double computeStandardError(double sigma1, int n1, double sigma2, int n2);

  // TODO: Factorize in IntervalStatistics
  Poincare::Layout m_estimateLayout;
  double m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H */
