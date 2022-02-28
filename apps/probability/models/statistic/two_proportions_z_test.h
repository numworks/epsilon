#ifndef PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_Z_TEST_H
#define PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_Z_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"

namespace Probability {

class TwoProportionsZTest : public Test {
friend class TwoProportions;
public:
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::TwoProportions; }
  DistributionType distributionType() const override { return DistributionType::Z; }
  I18n::Message title() const override { return TwoProportions::Title(); }
  I18n::Message graphTitleFormat() const override { return DistributionZ::GraphTitleFormat(); }

  // Significance Test: TwoProportions
  const char * hypothesisSymbol() override { return TwoProportions::HypothesisSymbol(); }
  void initParameters() override { TwoProportions::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return TwoProportions::AuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = TwoProportions::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }
  bool isValidH0(double h0) override { return TwoProportions::ValidH0(h0); }

  void compute() override { TwoProportions::ComputeTest(this); }

  // Distribution: z
  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override { return DistributionZ::TestCriticalValueSymbol(font); }
  float canonicalDensityFunction(float x) const override { return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionZ::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double * p) override { return DistributionZ::CumulativeNormalizedInverseDistributionFunction(*p, m_degreesOfFreedom); }

private:
  // Significance Test: TwoProportions
  bool validateInputs() override { return TwoProportions::ValidateInputs(m_params); }
  int numberOfStatisticParameters() const override { return TwoProportions::NumberOfParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override { return TwoProportions::ParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }
  // Distribution: z
  float computeYMax() const override { return DistributionZ::YMax(m_degreesOfFreedom); }

  double m_params[TwoProportions::k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_TWO_PROPORTIONS_Z_TEST_H */
