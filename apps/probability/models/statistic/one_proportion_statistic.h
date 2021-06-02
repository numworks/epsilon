#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H

#include "statistic.h"

namespace Probability
{

class OneProportionStatistic : public Statistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Test statistic
  const char * testCriticalValueSymbol() override { return "z"; };
  float testCriticalValue() override { return m_z; };
  float pValue() override { return m_pValue; };
  bool hasDegreeOfFreedom() override { return false; };

  // Confidence interval
  const char * estimateSymbol() override { return "p"; };
  I18n::Message estimateDescription() override { return I18n::Message::SampleProportion; };
  float estimate() override { return m_pEstimate; };
  const char * intervalCriticalValueSymbol() override { return "z*"; };
  float intervalCriticalValue() override { return m_zCritical; };
  float standardError() override { return m_SE; };
  float marginOfError() override { return m_ME; };

private:
  float _pEstimate(float x, float n);
  float _z(float p0, float p, int n);
  float _pVal(float z, char op);
  float _zCritical(float confidenceLevel);
  float _SE(float pEstimate, int n);

  float m_z;
  float m_pValue;
  float m_pEstimate;
  float m_zCritical;
  float m_SE;
  float m_ME;
};


} // namespace Probability


#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H */
