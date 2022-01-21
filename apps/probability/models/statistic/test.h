#ifndef PROBABILITY_MODELS_STATISTIC_TEST_H
#define PROBABILITY_MODELS_STATISTIC_TEST_H

#include "hypothesis_params.h"
#include "statistic.h"

namespace Probability {

class Test : public Statistic {
friend class SignificanceTest;
friend class OneMean;
friend class OneProportion;
friend class TwoMeans;
friend class PooledTwoMeans;
friend class TwoProportions;
public:
  Test() :
    Statistic(),
    m_testCriticalValue(NAN),
    m_pValue(NAN) {}

  void initializeSignificanceTest(SignificanceTestType type) override;
  void compute() override;

  // Evaluation
  float evaluateAtAbscissa(float x) const override { return canonicalDensityFunction(x); }
  void initThreshold() override { m_threshold = 0.05; }

  // Input
  HypothesisParams * hypothesisParams() { return &m_hypothesisParams; }
  virtual bool isValidH0(double h0) { return true; }

  // Test statistic
  /* Returns the abscissa on the normalized density curve
   * corresponding to the input sample. */
  double testCriticalValue() const { return m_testCriticalValue; };
  /* The p-value is the probability of obtaining a results at least
   * as extreme as what was observed with the sample */
  double pValue() const { return m_pValue; };
  /* Returns the value above/below (depending on the operator) which the probability
   * of landing is inferior to a given significance level. */
  bool canRejectNull();

  // Range
  constexpr static float k_displayWidthToSTDRatio = 5.f;

protected:
  float computeXMin() const override { return -k_displayWidthToSTDRatio; }
  float computeXMax() const override { return k_displayWidthToSTDRatio; }
  virtual void computeTest() = 0;
  // Hypothesis chosen
  HypothesisParams m_hypothesisParams;
  // Cached values
  double m_testCriticalValue;
  double m_pValue;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_TEST_H */
