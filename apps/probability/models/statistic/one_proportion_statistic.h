#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H

#include "statistic.h"
#include "probability/models/data.h"

namespace Probability
{

class OneProportionStatistic : public Statistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Test statistic
  const char * testCriticalValueSymbol() override { return "z"; };
  float testCriticalValue() override { return m_z; };
  void pValue() override { return m_pValue; };
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
  float _pVal(float z, Data::ComparisonOperator op);
  float _zCritical(float confidenceLevel);
  float _SE(float pEstimate, int n);

  int m_z;
  int m_pValue;
  int m_pEstimate;
  int m_zCritical;
  int m_SE;
  int m_ME;
};


} // namespace Probability


#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H */
