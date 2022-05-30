#ifndef INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H
#define INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"

namespace Inference {

class OneProportionZTest : public Test {
friend class OneProportion;
public:
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::OneProportion; }
  DistributionType distributionType() const override { return DistributionType::Z; }
  I18n::Message title() const override { return OneProportion::Title(); }
  I18n::Message graphTitleFormat() const override { return DistributionZ::GraphTitleFormat(); }
  void tidy() override { m_estimateLayout = Poincare::Layout(); }

  // Significance Test: OneProportion
  const char * hypothesisSymbol() override { return OneProportion::HypothesisSymbol(); }
  void initParameters() override { OneProportion::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && OneProportion::AuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = OneProportion::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }
  bool isValidH0(double h0) override { return OneProportion::ValidH0(h0); }

  void compute() override { OneProportion::ComputeTest(this); }

  // Estimates
  int numberOfEstimates() const override { return 1; }
  double estimateValue(int index) override { return OneProportion::X(parametersArray()) / OneProportion::N(parametersArray()); }
  Poincare::Layout estimateLayout(int index) const override { return OneProportion::EstimateLayout(&m_estimateLayout); }
  I18n::Message estimateDescription(int index) override { return OneProportion::EstimateDescription(); }

  // Distribution: z
  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override { return DistributionZ::TestCriticalValueSymbol(font); }
  float canonicalDensityFunction(float x) const override { return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionZ::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double p) const override { return DistributionZ::CumulativeNormalizedInverseDistributionFunction(p, m_degreesOfFreedom); }

private:
  // Significance Test: OneProportion
  bool validateInputs() override { return OneProportion::ValidateInputs(m_params); }
  int numberOfStatisticParameters() const override { return OneProportion::NumberOfParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override { return OneProportion::ParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }
  // Distribution: z
  float computeYMax() const override { return DistributionZ::YMax(m_degreesOfFreedom); }

  double m_params[OneProportion::k_numberOfParams];
  mutable Poincare::Layout m_estimateLayout;
};
}  // namespace Inference

#endif /* INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_TEST_H */
