#ifndef INFERENCE_MODELS_STATISTIC_TEST_H
#define INFERENCE_MODELS_STATISTIC_TEST_H

#include "statistic.h"

namespace Inference {

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
  SubApp subApp() const override { return SubApp::Test; }

  bool initializeSignificanceTest(SignificanceTestType type, Shared::GlobalContext * context) override;
  I18n::Message statisticTitle() const override { return I18n::Message::Tests; }
  I18n::Message statisticBasicTitle() const override { return I18n::Message::Test; }
  // TODO: factorize with intervals!
  I18n::Message tStatisticMessage() const override { return I18n::Message::TTest; }
  I18n::Message zStatisticMessage() const override { return I18n::Message::ZTest; }
  I18n::Message tOrZStatisticMessage() const override { return I18n::Message::TOrZTest; }
  I18n::Message tDistributionName() const override { return I18n::Message::TTest; }
  I18n::Message tPooledDistributionName() const override { return I18n::Message::PooledTTest; }
  I18n::Message zDistributionName() const override { return I18n::Message::ZTest; }
  void setGraphTitle(char * buffer, size_t bufferSize) const override;

  // Evaluation
  float evaluateAtAbscissa(float x) const override { return canonicalDensityFunction(x); }
  void initParameters() override { m_threshold = 0.05; }
  I18n::Message thresholdName() const override { return I18n::Message::GreekAlpha; }
  I18n::Message thresholdDescription() const override { return I18n::Message::SignificanceLevel; }

  // Input
  bool hasHypothesisParameters() const override { return true; }
  HypothesisParams * hypothesisParams() override { return &m_hypothesisParams; }
  virtual bool isValidH0(double h0) { return true; }

  // Additional estimates
  virtual int numberOfEstimates() const { return 0; }
  virtual double estimateValue(int index) { return 0; }
  virtual Poincare::Layout estimateLayout(int index) const { return nullptr; }
  virtual I18n::Message estimateDescription(int index) { return I18n::Message::Default; }

  // Test statistic
  /* Returns the abscissa on the normalized density curve
   * corresponding to the input sample. */
  double testCriticalValue() const { return m_testCriticalValue; }
  /* The p-value is the probability of obtaining a results at least
   * as extreme as what was observed with the sample */
  double pValue() const { return m_pValue; };
  /* Returns the value above/below (depending on the operator) which the probability
   * of landing is inferior to a given significance level. */
  bool canRejectNull();
  virtual double thresholdAbscissa(HypothesisParams::ComparisonOperator op) const;

  // Output
  int numberOfResults() const override { return 2 + numberOfEstimates() + hasDegreeOfFreedom(); }
  void resultAtIndex(int index, double * value, Poincare::Layout * message, I18n::Message * subMessage, int * precision) override;

  // Range
  constexpr static float k_displayWidthToSTDRatio = 5.f;

protected:
  float computeXMin() const override { return -k_displayWidthToSTDRatio; }
  float computeXMax() const override { return k_displayWidthToSTDRatio; }
  virtual I18n::Message graphTitleFormat() const { return I18n::Message::Default; }
  // Hypothesis chosen
  HypothesisParams m_hypothesisParams;
  // Cached values
  double m_testCriticalValue;
  double m_pValue;
private:
  enum ResultOrder { Z, PValue, TestDegree };
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_TEST_H */
