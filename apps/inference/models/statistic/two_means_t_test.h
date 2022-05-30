#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_TEST_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"

namespace Inference {

class TwoMeansTTest : public Test {
friend class TwoMeans;
public:
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::TwoMeans; }
  DistributionType distributionType() const override { return DistributionType::T; }
  I18n::Message title() const override { return TwoMeans::TTitle(); }
  I18n::Message graphTitleFormat() const override { return DistributionT::GraphTitleFormat(); }

  // Significance Test: TwoMeans
  bool initializeDistribution(DistributionType distributionType) override { return TwoMeans::TestInitializeDistribution(this, distributionType); }
  int numberOfAvailableDistributions() const override { return TwoMeans::NumberOfAvailableDistributions(); }
  I18n::Message distributionTitle() const override { return TwoMeans::DistributionTitle(); }
  const char * hypothesisSymbol() override { return TwoMeans::HypothesisSymbol(); }
  void initParameters() override { TwoMeans::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && TwoMeans::TAuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void compute() override { TwoMeans::ComputeTTest(this); }

  // Distribution: t
  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override { return DistributionT::TestCriticalValueSymbol(font); }
  float canonicalDensityFunction(float x) const override { return DistributionT::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionT::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double p) const override { return DistributionT::CumulativeNormalizedInverseDistributionFunction(p, m_degreesOfFreedom); }

private:
  // Significance Test: TwoMeans
  bool validateInputs() override { return TwoMeans::TValidateInputs(m_params); }
  int numberOfStatisticParameters() const override { return TwoMeans::NumberOfParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override { return TwoMeans::TParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }

  // Distribution: t
  float computeYMax() const override { return DistributionT::YMax(m_degreesOfFreedom); }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_TEST_H */
