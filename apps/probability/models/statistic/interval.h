#ifndef PROBABILITY_MODELS_STATISTIC_INTERVAL_H
#define PROBABILITY_MODELS_STATISTIC_INTERVAL_H

#include "statistic.h"

namespace Probability {

class Interval : public Statistic {
friend class SignificanceTest;
friend class OneMean;
friend class OneProportion;
friend class TwoMeans;
friend class PooledTwoMeans;
friend class TwoProportions;
public:
  Interval() :
    m_estimateLayout(),
    m_estimate(NAN),
    m_zCritical(NAN),
    m_SE(NAN), // Initialize to make sure m_SE != 0 by default and test-statistics are graphable.
    m_marginOfError(NAN) {}
  virtual ~Interval();

  void initializeSignificanceTest(SignificanceTestType type) override;
  void tidy();
  void compute() override;

  float evaluateAtAbscissa(float x) const override { return canonicalDensityFunction((x - estimate()) / standardError()); }
  void initThreshold() override { m_threshold = 0.95; }

  virtual const char * estimateSymbol() = 0;
  virtual Poincare::Layout estimateLayout() { return m_estimateLayout; }
  virtual I18n::Message estimateDescription() { return I18n::Message::Default; }
  /* The estimate is the center of the confidence interval,
   * and estimates the parameter of interest. */
  double estimate() const { return m_estimate; };
  Poincare::Layout intervalCriticalValueSymbol();
  /* Returns the critical value above which the probability
   * of landing is inferior to a given confidence level,
   * for the normalized distribution. */
  double intervalCriticalValue() const { return m_zCritical; };
  /* Returns the variance estimated from the sample. */
  double standardError() const { return m_SE; };
  /* Returns the half-width of the confidence interval. */
  double marginOfError() const { return m_marginOfError; };

  // CurveViewRange
  bool isGraphable() const override;

  /* Display */
  constexpr static int k_numberOfDisplayedIntervals = 4;
  // Return the interval threshold to display at given index
  static float DisplayedIntervalThresholdAtIndex(float threshold, int index);
  // Return the displayed position of the main threshold interval
  static int MainDisplayedIntervalThresholdIndex(float mainThreshold);
protected:
  constexpr static float k_intervalMarginRatio = 1.25f;
  float computeXMin() const override;
  float computeXMax() const override;

  double computeIntervalCriticalValue();
  virtual void computeInterval() = 0;

  Poincare::Layout m_estimateLayout;
  double m_estimate;
  double m_zCritical;
  double m_SE;
  double m_marginOfError;
private:
  float largestMarginOfError();
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_INTERVAL_H */
