#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H

#include <apps/i18n.h>

namespace Probability {

/* A Statistic is something that is computed from a sample and whose distribution is known.
 * From its distribution, we can compute statistical test results and confidence intervals.
 */
class Statistic {
public:
  virtual ~Statistic() = default;

  virtual void computeTest() = 0;
  virtual void computeInterval() = 0;

  virtual float normedDensityFunction(float x) = 0;
  virtual float densityFunction(float x) {return 0;}  // TODO

  // Test statistic
  virtual const char * testCriticalValueSymbol() = 0;
  virtual float testCriticalValue() = 0;
  virtual float pValue() = 0;
  virtual bool hasDegreeOfFreedom() = 0;
  virtual float degreeOfFreedom() { return -1; };
  bool testPassed();

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
  /* Returns the critical value above which the probability
   * of landing is inferior to a given significance level. */
  float testCriticalValue() override { return m_z; };
  /* The p-value is the probability of obtaining a results at least
   * as extreme as what was observed with the sample */
  float pValue() override { return m_pValue; };

  // Confidence interval
  /* The estimate is the center of the confidence interval,
   * and estimates the parameter of interest. */
  float estimate() override { return m_pEstimate; };
  /* Returns the critical value above which the probability
   * of landing is inferior to a given confidence level,
   *  for the normed distribution. */
  float intervalCriticalValue() override { return m_zCritical; };
  /* Returns the variance estimated from the sample. */
  float standardError() override { return m_SE; };
  /* Returns the half-width of the confidence interval. */
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
