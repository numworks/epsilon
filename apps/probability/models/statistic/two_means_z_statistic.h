#ifndef APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class TwoMeansZStatistic : public ZStatistic {
public:
  bool isValidParamAtIndex(int i, float p) override;
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x1-x2"; };
  I18n::Message estimateDescription() override { return I18n::Message::Default; };

protected:
  // Parameters
  constexpr static int k_numberOfParams = 6;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X1, N1, Sigma1, X2, N2, Sigma2 };
  ParameterRepr paramReprAtIndex(int i) const override;
  float * paramArray() override { return m_params; }

private:
  float x1() { return m_params[ParamsOrder::X1]; }
  float n1() { return m_params[ParamsOrder::N1]; }
  float sigma1() { return m_params[ParamsOrder::Sigma1]; }
  float x2() { return m_params[ParamsOrder::X2]; }
  float n2() { return m_params[ParamsOrder::N2]; }
  float sigma2() { return m_params[ParamsOrder::Sigma2]; }


  float _xEstimate(float meanSample1, float meanSample2);
  float _z(float deltaMean, float meanSample1, float n1, float sigma1, float meanSample2, float n2,
           float sigma2);
  float _SE(float sigma1, int n1, float sigma2, int n2);

  float m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H */
