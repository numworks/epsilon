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
  double testCriticalValue() const override { return m_testCriticalValue; };
  double pValue() const override { return m_pValue; };

  // Confidence interval
  double estimate() const override { return m_estimate; };
  double intervalCriticalValue() const override { return m_zCritical; };
  double standardError() const override { return m_SE; };
  double marginOfError() const override { return m_marginOfError; };

protected:
  double computeMarginOfError(double zCritical, double SE) { return zCritical * SE; }

  double m_testCriticalValue;
  double m_pValue;
  double m_estimate;
  double m_zCritical;
  double m_SE;
  double m_marginOfError;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_CACHED_STATISTIC_H */
