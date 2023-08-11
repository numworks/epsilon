#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_Z_INTERVAL_H

#include "interval.h"

namespace Inference {

class TwoMeansZInterval : public Interval {
  friend class TwoMeans;

 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::TwoMeans;
  }
  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
  I18n::Message title() const override { return TwoMeans::ZTitle(); }
  // Significance Test: TwoMeans
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
           TwoMeans::ZAuthorizedParameterAtIndex(i, p);
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
  bool validateInputs() override { return TwoMeans::ZValidateInputs(m_params); }
  int numberOfStatisticParameters() const override {
    return TwoMeans::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return TwoMeans::ZParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }
  void privateCompute() override { TwoMeans::ComputeZInterval(this); }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Inference

#endif
