#ifndef APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_STATISTIC_H

#include "t_statistic.h"

namespace Probability {

class TwoMeansTStatistic : public TStatistic {
public:
  TwoMeansTStatistic();
  bool isValidParamAtIndex(int i, float p) override;
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
  enum ParamsOrder { X1, S1, N1, X2, S2, N2 };
  ParameterRepr paramReprAtIndex(int i) const override;
  float * paramArray() override { return m_params; }

private:
  float x1() { return m_params[ParamsOrder::X1]; }
  float n1() { return m_params[ParamsOrder::N1]; }
  float s1() { return m_params[ParamsOrder::S1]; }
  float x2() { return m_params[ParamsOrder::X2]; }
  float n2() { return m_params[ParamsOrder::N2]; }
  float s2() { return m_params[ParamsOrder::S2]; }

  float _xEstimate(float meanSample1, float meanSample2);
  float _t(float deltaMean, float meanSample1, float n1, float sigma1, float meanSample2, float n2,
           float sigma2);
  virtual float _degreeOfFreedom(float s1, int n1, float s2, int n2);
  virtual float _SE(float s1, int n1, float s2, int n2);

  float m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_STATISTIC_H */
