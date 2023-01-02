#ifndef INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_ONE_PROPORTION_Z_INTERVAL_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "interval.h"

namespace Inference {

class OneProportionZInterval : public Interval {
friend class OneProportion;
public:
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::OneProportion; }
  DistributionType distributionType() const override { return DistributionType::Z; }
  I18n::Message title() const override { return OneProportion::Title(); }
  // Significance Test: OneProportion
  void initParameters() override { OneProportion::InitIntervalParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && OneProportion::AuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = OneProportion::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }

  void compute() override { OneProportion::ComputeInterval(this); }

  const char * estimateSymbol() const override { return OneProportion::EstimateSymbol(); }
  Poincare::Layout testCriticalValueSymbol() override { return DistributionZ::TestCriticalValueSymbol(); }
  Poincare::Layout estimateLayout() const override { return OneProportion::EstimateLayout(&m_estimateLayout); }
  I18n::Message estimateDescription() override { return OneProportion::EstimateDescription(); };

  // Distribution: z
  float canonicalDensityFunction(float x) const override { return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionZ::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double p) const override { return DistributionZ::CumulativeNormalizedInverseDistributionFunction(p, m_degreesOfFreedom); }

private:
  // Significance Test: OneProportion
  bool validateInputs() override { return OneProportion::ValidateInputs(m_params); }
  int numberOfStatisticParameters() const override { return OneProportion::NumberOfParameters(); }
  Shared::ParameterRepresentation paramRepresentationAtIndex(int i) const override { return OneProportion::ParameterRepresentationAtIndex(i); }
  double * parametersArray() override { return m_params; }

    // Distribution: z
  float computeYMax() const override { return DistributionZ::YMax(m_degreesOfFreedom); }

  double m_params[OneProportion::k_numberOfParams];
};

}

#endif
