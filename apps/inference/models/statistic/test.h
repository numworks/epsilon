#ifndef INFERENCE_MODELS_STATISTIC_TEST_H
#define INFERENCE_MODELS_STATISTIC_TEST_H

#include <poincare/statistics/inference.h>

#include "statistic.h"

namespace Inference {

class Test : public Statistic {
 public:
  Test() : Statistic(), m_testCriticalValue(NAN), m_pValue(NAN) {}

  SubApp subApp() const override { return SubApp::SignificanceTest; }

  void setGraphTitle(char* buffer, size_t bufferSize) const override;

  // Evaluation
  float evaluateAtAbscissa(float x) const override {
    return canonicalDensityFunction(x);
  }

  // Input
  PcrInference::Hypothesis* hypothesis() { return &m_hypothesis; }
  const PcrInference::Hypothesis* hypothesis() const { return &m_hypothesis; }
  const char* hypothesisSymbol() const {
    return PcrInference::HypothesisSymbol(testType());
  }
  bool isValidH0(double h0) { return PcrInference::IsH0Valid(testType(), h0); }

  void initParameters() override;

  // Additional estimates
  int numberOfEstimates() const {
    return PcrInference::NumberOfTestEstimates(testType());
  };
  double estimateValue(int index) { return m_estimates[index]; }
  Poincare::Layout estimateLayout(int index) const {
    return PcrInference::TestEstimateLayout(testType(), index);
  }
  I18n::Message estimateDescription(int index) const {
    return TestEstimateDescription(testType(), index);
  }

  // Test statistic
  /* Returns the abscissa on the normalized density curve
   * corresponding to the input sample. */
  double testCriticalValue() const { return m_testCriticalValue; }
  /* The p-value is the probability of obtaining a results at least
   * as extreme as what was observed with the sample */
  double pValue() const { return m_pValue; };
  /* Returns the value above/below (depending on the operator) which the
   * probability of landing is inferior to a given significance level. */
  bool canRejectNull();
  double thresholdAbscissa(Poincare::ComparisonJunior::Operator op,
                           double factor = 1.0) const;
  bool isGraphable() const override {
    return std::isfinite(m_testCriticalValue) && std::isfinite(m_pValue);
  }

  // Output
  void compute() override;

  // Range
  constexpr static float k_displayWidthToSTDRatio = 5.f;
  constexpr static float k_displayZoomedInTopMarginRatio = 0.2f;
  constexpr static float k_displayZoomedInHorizontalMarginRatio = 0.3f;
  bool computeCurveViewRange(float transition, bool zoomSide);
  bool hasTwoSides();

 protected:
  int numberOfInferenceResults() const override {
    return 2 + numberOfEstimates() + hasDegreeOfFreedom();
  }
  void inferenceResultAtIndex(int index, double* value,
                              Poincare::Layout* message,
                              I18n::Message* subMessage,
                              int* precision) override;

  float computeXMin() const override { return -k_displayWidthToSTDRatio; }
  float computeXMax() const override { return k_displayWidthToSTDRatio; }
  // Hypothesis chosen
  PcrInference::Hypothesis m_hypothesis;
  // Cached values
  PcrInference::Estimates m_estimates;
  double m_testCriticalValue;
  double m_pValue;

 private:
  enum ResultOrder { Z, PValue, TestDegree };
  virtual bool shouldForbidZoom(float alpha, float criticalValue);
};

}  // namespace Inference

#endif
