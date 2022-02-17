#ifndef PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_INTERVAl_H
#define PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_INTERVAl_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "interval.h"

namespace Probability {

class TwoMeansTInterval : public Interval {
friend class TwoMeans;
public:
  void initializeDistribution(DistributionType distribution) override;
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::TwoMeans; }
  DistributionType distributionType() const override { return DistributionType::T; }
  I18n::Message title() const override { return TwoMeans::TTitle(); }

  // Significance Test: TwoMeans Mean
  int numberOfAvailableDistributions() const override { return TwoMeans::NumberOfAvailableDistributions(); }
  I18n::Message distributionTitle() const override { return TwoMeans::DistributionTitle(); }
  I18n::Message distributionDescription() const override { return TwoMeans::IntervalDistributionDescription(); }
  void initParameters() override { TwoMeans::InitIntervalParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override { return TwoMeans::TAuthorizedParameterAtIndex(i, p); }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }

  void computeInterval() override { TwoMeans::ComputeTInterval(this); }

  const char * estimateSymbol() override { return TwoMeans::EstimateSymbol(); }
  Poincare::Layout estimateLayout() override { return TwoMeans::EstimateLayout(&m_estimateLayout); }
  Poincare::Layout testCriticalValueSymbol() override { return DistributionT::TestCriticalValueSymbol(); }
  I18n::Message estimateDescription() override { return TwoMeans::EstimateDescription(); };
  // Distribution: t
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

#endif /* PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_INTERVAl_H */
