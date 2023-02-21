#ifndef INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "interval.h"

namespace Inference {

class TwoProportionsZInterval : public Interval {
  friend class TwoProportions;

 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::TwoProportions;
  }
  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
  I18n::Message title() const override { return TwoProportions::Title(); }
  // Significance Test: TwoProportions
  void initParameters() override {
    TwoProportions::InitIntervalParameters(this);
  }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           TwoProportions::AuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = TwoProportions::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }

  void compute() override { TwoProportions::ComputeInterval(this); }

  const char* estimateSymbol() const override {
    return TwoProportions::EstimateSymbol();
  }
  Poincare::Layout estimateLayout() const override {
    return TwoProportions::EstimateLayout(&m_estimateLayout);
  }
  Poincare::Layout testCriticalValueSymbol() override {
    return DistributionZ::TestCriticalValueSymbol();
  }
  I18n::Message estimateDescription() override {
    return TwoProportions::EstimateDescription();
  };

  // Distribution: z
  float canonicalDensityFunction(float x) const override {
    return DistributionZ::CanonicalDensityFunction(x, m_degreesOfFreedom);
  }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override {
    return DistributionZ::CumulativeNormalizedDistributionFunction(
        x, m_degreesOfFreedom);
  }
  double cumulativeDistributiveInverseForProbability(double p) const override {
    return DistributionZ::CumulativeNormalizedInverseDistributionFunction(
        p, m_degreesOfFreedom);
  }

 private:
  // Significance Test: TwoProportions
  bool validateInputs() override {
    return TwoProportions::ValidateInputs(m_params);
  }
  int numberOfStatisticParameters() const override {
    return TwoProportions::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return TwoProportions::ParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }

  // Distribution: z
  float computeYMax() const override {
    return DistributionZ::YMax(m_degreesOfFreedom);
  }

  double m_params[TwoProportions::k_numberOfParams];
};

}  // namespace Inference

#endif
