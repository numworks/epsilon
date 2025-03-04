#ifndef INFERENCE_MODELS_STATISTIC_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_INTERVAL_H

#include <poincare/statistics/inference.h>

#include "statistic.h"

namespace Inference {

class Interval : public Statistic {
  friend class SignificanceTest;
  friend class OneMean;
  friend class OneProportion;
  friend class TwoMeans;
  friend class TwoProportions;

 public:
  Interval()
      : m_estimate(NAN),
        /* Initialize to make sure m_SE != 0 by default and test-statistics are
         * graphable. */
        m_SE(NAN),
        m_zCritical(NAN),
        m_marginOfError(NAN) {}
  virtual ~Interval();
  SubApp subApp() const override { return SubApp::ConfidenceInterval; }

  void setGraphTitle(char* buffer, size_t bufferSize) const override final {
    setGraphTitleForValue(marginOfError(), buffer, bufferSize);
  }
  void setGraphTitleForValue(double marginOfError, char* buffer,
                             size_t bufferSize) const;
  void setResultTitle(char* buffer, size_t bufferSize,
                      bool resultIsTopPage) const override final {
    setResultTitleForValues(estimate(), threshold(), buffer, bufferSize,
                            resultIsTopPage);
  }
  void setResultTitleForValues(double estimate, double threshold, char* buffer,
                               size_t bufferSize, bool resultIsTopPage) const;

  float evaluateAtAbscissa(float x) const override {
    return canonicalDensityFunction((x - estimate()) / standardError());
  }

  bool showEstimate() const {
    return PcrInference::ShowIntervalEstimate(testType());
  }
  const char* estimateSymbol() const {
    return PcrInference::IntervalEstimateSymbol(testType());
  }
  Poincare::Layout estimateLayout() const {
    return PcrInference::IntervalEstimateLayout(type());
  }
  I18n::Message estimateDescription() const {
    return IntervalEstimateDescription(testType());
  }
  /* The estimate is the center of the confidence interval,
   * and estimates the parameter of interest. */
  double estimate() const { return m_estimate; };
  /* Returns the critical value above which the probability
   * of landing is inferior to a given confidence level,
   * for the normalized distribution. */
  double intervalCriticalValue() const { return m_zCritical; };
  /* Returns the variance estimated from the sample. */
  double standardError() const {
    assert(std::isnan(m_SE) || m_SE >= 0);
    return m_SE;
  };
  /* Returns the half-width of the confidence interval. */
  double marginOfError() const {
    assert(std::isnan(m_marginOfError) || m_marginOfError >= 0);
    return m_marginOfError;
  };

  // Output
  void compute() override;

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

  double m_estimate;
  double m_SE;

 private:
  enum ResultOrder { Estimate, Critical, SE, ME, IntervalDegree };
  int numberOfInferenceResults() const override {
    return 3 + hasDegreeOfFreedom() + showEstimate();
  }
  void inferenceResultAtIndex(int index, double* value,
                              Poincare::Layout* message,
                              I18n::Message* subMessage,
                              int* precision) override;

  float largestMarginOfError();

  double m_zCritical;
  double m_marginOfError;
};

}  // namespace Inference

#endif
