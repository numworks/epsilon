#ifndef PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H

#include "statistic.h"

namespace Probability {

/* Store computed results as members to avoid recomputing at every call */
class CachedStatistic : public Statistic {
public:
  // Test statistic
  float testCriticalValue() const override { return m_testCriticalValue; };
  float pValue() const override { return m_pValue; };
  float zAlpha() const override { return m_zAlpha; }

  // Confidence interval
  float estimate() const override { return m_estimate; };
  float intervalCriticalValue() const override { return m_zCritical; };
  float standardError() const override { return m_SE; };
  float marginOfError() const override { return m_marginOfError; };

protected:
  float computeMarginOfError(float zCritical, float SE) { return zCritical * SE; }

  float m_testCriticalValue;
  float m_zAlpha;
  float m_pValue;
  float m_estimate;
  float m_zCritical;
  float m_SE;
  float m_marginOfError;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H */
