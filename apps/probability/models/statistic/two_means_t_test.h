#ifndef PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_TEST_H
#define PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"

namespace Probability {

class TwoMeansTTest : public Test {
friend class TwoMeans;
public:
  void initializeDistribution(DistributionType distribution) override;
  I18n::Message title() const override { return TwoMeans::TTitle(); }

  // Significance Test: TwoMeans
  void initParameters() override { TwoMeans::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return TwoMeans::TAuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void computeTest() override { TwoMeans::ComputeTTest(this); }

  // Distribution: t
  Poincare::Layout testCriticalValueSymbol() override { return DistributionT::TestCriticalValueSymbol(); }
  float canonicalDensityFunction(float x) const override { return DistributionT::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionT::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double * p) override { return DistributionT::CumulativeNormalizedInverseDistributionFunction(*p, m_degreesOfFreedom); }

private:
  // Significance Test: TwoMeans
  bool validateInputs() override { return TwoMeans::TValidateInputs(m_params); }
  int numberOfStatisticParameters() const override { return TwoMeans::NumberOfParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override { return TwoMeans::ParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }

  // Distribution: t
  float computeYMax() const override { return DistributionT::YMax(m_degreesOfFreedom); }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_TEST_H */
