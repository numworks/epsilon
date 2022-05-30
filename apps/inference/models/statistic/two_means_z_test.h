#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_Z_TEST_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_Z_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"

namespace Inference {

class TwoMeansZTest : public Test {
friend class TwoMeans;
public:
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::TwoMeans; }
  DistributionType distributionType() const override { return DistributionType::Z; }
  I18n::Message title() const override { return TwoMeans::ZTitle(); }
  I18n::Message graphTitleFormat() const override { return DistributionZ::GraphTitleFormat(); }

  // Significance Test: TwoMeans
  bool initializeDistribution(DistributionType distributionType) override { return TwoMeans::TestInitializeDistribution(this, distributionType); }
  int numberOfAvailableDistributions() const override { return TwoMeans::NumberOfAvailableDistributions(); }
  I18n::Message distributionTitle() const override { return TwoMeans::DistributionTitle(); }
  const char * hypothesisSymbol() override { return TwoMeans::HypothesisSymbol(); }
  void initParameters() override { TwoMeans::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && TwoMeans::ZAuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void compute() override { TwoMeans::ComputeZTest(this); }

  // Distribution: z
  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override { return DistributionZ::TestCriticalValueSymbol(font); }
  float canonicalDensityFunction(float x) const override { return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionZ::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double p) const override { return DistributionZ::CumulativeNormalizedInverseDistributionFunction(p, m_degreesOfFreedom); }

private:
  // Significance Test: TwoMeans
  bool validateInputs() override { return TwoMeans::ZValidateInputs(m_params); }
  int numberOfStatisticParameters() const override { return TwoMeans::NumberOfParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override { return TwoMeans::ZParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }

  // Distribution: z
  float computeYMax() const override { return DistributionZ::YMax(m_degreesOfFreedom); }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_TWO_MEANS_Z_TEST_H */
