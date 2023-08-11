#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_INTERVAl_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_T_INTERVAl_H

#include "interval.h"

namespace Inference {

class TwoMeansTInterval : public Interval {
  friend class TwoMeans;

 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::TwoMeans;
  }
  DistributionType distributionType() const override {
    return DistributionType::T;
  }
  I18n::Message title() const override { return TwoMeans::TTitle(); }

  // Significance Test: TwoMeans Mean
  bool initializeDistribution(DistributionType distributionType) override {
    return TwoMeans::IntervalInitializeDistribution(this, distributionType);
  }
  int numberOfAvailableDistributions() const override {
    return TwoMeans::NumberOfAvailableDistributions();
  }
  I18n::Message distributionTitle() const override {
    return TwoMeans::DistributionTitle();
  }
  void initParameters() override { TwoMeans::InitIntervalParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           TwoMeans::TAuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = TwoMeans::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }

  const char* estimateSymbol() const override {
    return TwoMeans::EstimateSymbol();
  }
  Poincare::Layout estimateLayout() const override {
    return TwoMeans::EstimateLayout(&m_estimateLayout);
  }
  I18n::Message estimateDescription() override {
    return TwoMeans::EstimateDescription();
  }

 private:
  // Significance Test: TwoMeans
  bool validateInputs() override { return TwoMeans::TValidateInputs(m_params); }
  int numberOfStatisticParameters() const override {
    return TwoMeans::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return TwoMeans::TParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }

  void privateCompute() override { TwoMeans::ComputeTInterval(this); }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Inference

#endif
