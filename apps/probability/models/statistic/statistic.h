#ifndef APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H

#include <apps/i18n.h>

#include "probability/models/data_enums.h"
#include "probability/models/hypothesis_params.h"

namespace Probability {

struct ParameterRepr {
  I18n::Message m_symbol;
  I18n::Message m_description;
};

/* A Statistic is something that is computed from a sample and whose distribution is known.
 * From its distribution, we can compute statistical test results and confidence intervals.
 */
class Statistic {
public:
  Statistic() : m_threshold(-1) {}
  virtual ~Statistic() = default;

  virtual void computeTest() = 0;
  virtual void computeInterval() = 0;

  virtual float normedDensityFunction(float x) = 0;
  virtual float densityFunction(float x) {
    return normedDensityFunction((x - estimate()) / standardError());
  };

  // Input
  int numberOfParameters() { return numberOfStatisticParameters() + 1 /* threshold */; }
  float paramAtIndex(int i);
  void setParamAtIndex(int i, float p);
  I18n::Message paramSymbolAtIndex(int i) const { return paramReprAtIndex(i)->m_symbol; }
  I18n::Message paramDescriptionAtIndex(int i) const { return paramReprAtIndex(i)->m_description; }
  float threshold() const { return m_threshold; }
  void setThreshold(float s) { m_threshold = s; }
  HypothesisParams * hypothesisParams() { return &m_hypothesisParams; }

  // Test statistic
  virtual const char * testCriticalValueSymbol() = 0;
  /* Returns the abscissa on the normed density curve
   * corresponding to the input sample. */
  virtual float testCriticalValue() = 0;
  /* The p-value is the probability of obtaining a results at least
   * as extreme as what was observed with the sample */
  virtual float pValue() = 0;
  virtual bool hasDegreeOfFreedom() = 0;
  virtual float degreeOfFreedom() { return -1; };
  /* Returns the value above which the probability
   * of landing is inferior to a given significance level. */
  virtual float zAlpha() = 0;
  bool testPassed();

  // Confidence interval
  virtual const char * estimateSymbol() = 0;
  virtual I18n::Message estimateDescription() = 0;
  /* The estimate is the center of the confidence interval,
   * and estimates the parameter of interest. */
  virtual float estimate() = 0;
  virtual const char * intervalCriticalValueSymbol() = 0;
  /* Returns the critical value above which the probability
   * of landing is inferior to a given confidence level,
   *  for the normed distribution. */
  virtual float intervalCriticalValue() = 0;
  /* Returns the variance estimated from the sample. */
  virtual float standardError() = 0;
  /* Returns the half-width of the confidence interval. */
  virtual float marginOfError() = 0;

  int indexOfThreshold() { return numberOfStatisticParameters(); }
  void initThreshold(Data::SubApp subapp);
  /* Instanciate correct Statistic bases on Test and TestType. */
  static void initializeStatistic(Statistic * statistic, Data::Test t, Data::TestType type);

protected:
  virtual int numberOfStatisticParameters() const = 0;
  virtual const ParameterRepr * paramReprAtIndex(int i) const = 0;
  virtual float * paramArray() = 0;
  float absIfNeeded(float f);

  /* Threshold is either the confidence level or the significance level */
  float m_threshold;
  /* Hypothesis chosen */
  HypothesisParams m_hypothesisParams;
};

/* Store computed results as members to avoid recomputing at every call */
class CachedStatistic : public Statistic {
public:
  // Test statistic
  float testCriticalValue() override { return m_z; };
  float pValue() override { return m_pValue; };
  float zAlpha() override { return m_zAlpha; }

  // Confidence interval
  float estimate() override { return m_pEstimate; };
  float intervalCriticalValue() override { return m_zCritical; };
  float standardError() override { return m_SE; };
  float marginOfError() override { return m_ME; };

protected:
  float _ME(float zCritical, float SE) { return zCritical * SE; }

  float m_z;
  float m_zAlpha;
  float m_pValue;
  float m_pEstimate;
  float m_zCritical;
  float m_SE;
  float m_ME;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_STATISTIC_H */
