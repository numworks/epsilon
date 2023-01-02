#ifndef INFERENCE_MODELS_STATISTIC_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_INTERVAL_H

#include "statistic.h"

namespace Inference {

class Interval : public Statistic {
friend class SignificanceTest;
friend class OneMean;
friend class OneProportion;
friend class TwoMeans;
friend class PooledTwoMeans;
friend class TwoProportions;
public:
  Interval() :
    m_estimate(NAN),
    m_zCritical(NAN),
    m_SE(NAN), // Initialize to make sure m_SE != 0 by default and test-statistics are graphable.
    m_marginOfError(NAN) {}
  virtual ~Interval();
  SubApp subApp() const override { return SubApp::Interval; }

  bool initializeSignificanceTest(SignificanceTestType type, Shared::GlobalContext * context) override;
  void tidy() override;
  I18n::Message statisticTitle() const override { return I18n::Message::IntervalDescr; }
  I18n::Message statisticBasicTitle() const override { return I18n::Message::Interval; }
  // Don't show Categorical cell for Interval
  int numberOfSignificancesTestTypes() const override { return k_numberOfSignificanceTestType - 1; }
  // TODO: factorize with tests!
  I18n::Message tStatisticMessage() const override { return I18n::Message::TInterval; }
  I18n::Message zStatisticMessage() const override { return I18n::Message::ZInterval; }
  I18n::Message tOrZStatisticMessage() const override { return I18n::Message::TOrZInterval; }
  I18n::Message tDistributionName() const override { return I18n::Message::TInterval; }
  I18n::Message tPooledDistributionName() const override { return I18n::Message::PooledTInterval; }
  I18n::Message zDistributionName() const override { return I18n::Message::ZInterval; }
  void setGraphTitle(char * buffer, size_t bufferSize) const override final {
    setGraphTitleForValue(marginOfError(), buffer, bufferSize);
  }
  void setGraphTitleForValue(double marginOfError, char * buffer, size_t bufferSize) const;
  void setResultTitle(char * buffer, size_t bufferSize, bool resultIsTopPage) const override final {
    setResultTitleForValues(estimate(), threshold(), buffer, bufferSize, resultIsTopPage);
  }
  void setResultTitleForValues(double estimate, double threshold, char * buffer, size_t bufferSize, bool resultIsTopPage) const;

  float evaluateAtAbscissa(float x) const override { return canonicalDensityFunction((x - estimate()) / standardError()); }
  void initParameters() override { m_threshold = 0.95; }
  I18n::Message thresholdName() const override { return I18n::Message::ConfidenceLevel; }
  I18n::Message thresholdDescription() const override { return I18n::Message::Default; }

  virtual const char * estimateSymbol() const = 0;
  virtual Poincare::Layout estimateLayout() const { return m_estimateLayout; }
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

  // Output
  int numberOfResults() const override { return 3 + hasDegreeOfFreedom() + !estimateLayout().isUninitialized(); }
  void resultAtIndex(int index, double * value, Poincare::Layout * message, I18n::Message * subMessage, int * precision) override;

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

  mutable Poincare::Layout m_estimateLayout;
  double m_estimate;
  double m_zCritical;
  double m_SE;
  double m_marginOfError;
private:
  enum ResultOrder { Estimate, Critical, SE, ME, IntervalDegree };
  float largestMarginOfError();
};

}

#endif
