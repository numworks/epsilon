#ifndef PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_TEST_H
#define PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"

namespace Probability {

class OneMeanZTest : public Test {
public:
  I18n::Message title() const override { return OneMean::ZTitle(); }
  // Significance Test: OneMean
  void initParameters() override { OneMean::InitTestParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return OneMean::ZAuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParamaterForIndex(p, index);
    Test::setParameterAtIndex(p, index);
  }

  void computeTest() override { OneMean::ComputeZTest(this); }

  // Distribution: z
  Poincare::Layout testCriticalValueSymbol() override { return DistributionZ::TestCriticalValueSymbol(); }
  float canonicalDensityFunction(float x) const override { return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionZ::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double * p) override { return DistributionZ::CumulativeNormalizedInverseDistributionFunction(*p, m_degreesOfFreedom); }

private:
  // Significance Test
  int numberOfStatisticParameters() const override { return OneMean::NumberOfParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override { return OneMean::ParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }

  // Distribution: z
  float computeYMax() const override { return DistributionZ::YMax(m_degreesOfFreedom); }

  double m_params[OneMean::k_numberOfParams];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_TEST_H */
