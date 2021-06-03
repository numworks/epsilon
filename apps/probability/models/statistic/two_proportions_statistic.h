#ifndef APPS_PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H

#include "proportion_statistic.h"

namespace Probability {

class TwoProportionsStatistic : public ProportionStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Test statistic
  const char * testCriticalValueSymbol() override { return "z"; };
  bool hasDegreeOfFreedom() override { return false; };

  // Confidence interval
  const char * estimateSymbol() override { return "p1-p2"; };
  I18n::Message estimateDescription() override { return I18n::Message::DiffSampleProportion; };
  const char * intervalCriticalValueSymbol() override { return "z*"; };

private:
  float _pEstimate(float x1, float n1, float x2, float n2);
  float _z(float deltaP0, float x1, int n1, float x2, int n2);
  float _SE(float p1Estimate, int n1, float p2Estimate, int n2);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_STATISTIC_H */
