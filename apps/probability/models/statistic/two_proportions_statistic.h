#ifndef APPS_PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class TwoProportionsStatistic : public ZStatistic {
public:
  TwoProportionsStatistic();
  bool isValidH0(float p) override;
  bool isValidParamAtIndex(int i, float p) override;
  void computeTest() override;
  void computeInterval() override;

  // Test statistic
  bool hasDegreeOfFreedom() override { return false; };

  // Confidence interval
  const char * estimateSymbol() override { return "p1-p2"; };
  Poincare::Layout estimateLayout() override;
  I18n::Message estimateDescription() override { return I18n::Message::SampleTwoProportions; };

protected:
  // Parameters
  constexpr static int k_numberOfParams = 4;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X1, N1, X2, N2 };
  ParameterRepr paramReprAtIndex(int i) const override;
  float * paramArray() override { return m_params; }

private:
  float x1() { return m_params[ParamsOrder::X1]; }
  float n1() { return m_params[ParamsOrder::N1]; }
  float x2() { return m_params[ParamsOrder::X2]; }
  float n2() { return m_params[ParamsOrder::N2]; }

  // Computation
  float _pEstimate(float x1, float n1, float x2, float n2);
  float _z(float deltaP0, float x1, int n1, float x2, int n2);
  float _SE(float p1Estimate, int n1, float p2Estimate, int n2);

  float m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H */
