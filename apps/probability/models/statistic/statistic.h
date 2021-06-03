#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H

#include <apps/i18n.h>

namespace Probability {

class Statistic {
public:
  virtual ~Statistic() = default;

  virtual void computeTest() = 0;
  virtual void computeInterval() = 0;

  // Test statistic
  virtual const char * testCriticalValueSymbol() = 0;
  virtual float testCriticalValue() = 0;
  virtual float pValue() = 0;
  virtual bool hasDegreeOfFreedom() = 0;
  virtual float degreeOfFreedom() { return -1; };

  // Confidence interval
  virtual const char * estimateSymbol() = 0;
  virtual I18n::Message estimateDescription() = 0;
  virtual float estimate() = 0;
  virtual const char * intervalCriticalValueSymbol() = 0;
  virtual float intervalCriticalValue() = 0;
  virtual float standardError() = 0;
  virtual float marginOfError() = 0;
};

/* Store computed results as members to avoid recomputing at every call */
class CachedStatistic : public Statistic {
public:
  // Test statistic
  float testCriticalValue() override { return m_z; };
  float pValue() override { return m_pValue; };

  // Confidence interval
  float estimate() override { return m_pEstimate; };
  float intervalCriticalValue() override { return m_zCritical; };
  float standardError() override { return m_SE; };
  float marginOfError() override { return m_ME; };

protected:
  float _ME(float zCritical, float SE) { return zCritical * SE; }

  float m_z;
  float m_pValue;
  float m_pEstimate;
  float m_zCritical;
  float m_SE;
  float m_ME;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H */
