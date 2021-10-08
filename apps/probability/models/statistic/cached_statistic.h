#ifndef PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H

#include "statistic.h"

namespace Probability {

/* Store computed results as members to avoid recomputing at every call */
class CachedStatistic : public Statistic {
public:
  CachedStatistic() :
    m_testCriticalValue(NAN),
    m_pValue(NAN),
    m_estimate(NAN),
    m_zCritical(NAN),
    m_SE(NAN), // Initialize to make sure m_SE != 0 by default and test-statistics are graphable.
    m_marginOfError(NAN) {}

  // Test statistic
  float testCriticalValue() const override { return m_testCriticalValue; };
  float pValue() const override { return m_pValue; };

  // Confidence interval
  float estimate() const override { return m_estimate; };
  float intervalCriticalValue() const override { return m_zCritical; };
  float standardError() const override { return m_SE; };
  float marginOfError() const override { return m_marginOfError; };

protected:
  float computeMarginOfError(float zCritical, float SE) { return zCritical * SE; }

  float m_testCriticalValue;
  float m_pValue;
  float m_estimate;
  float m_zCritical;
  float m_SE;
  float m_marginOfError;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H */
