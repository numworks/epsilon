#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_Z_INTERVAL_H

#include "interval.h"

namespace Inference {

class OneMeanZInterval : public Interval {
 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
  I18n::Message title() const override { return OneMean::ZTitle(); }
  // Significance Test: OneMean
  bool initializeDistribution(DistributionType distributionType) override {
    return OneMean::IntervalInitializeDistribution(this, distributionType);
  }
  int numberOfAvailableDistributions() const override {
    return OneMean::NumberOfAvailableDistributions();
  }
  I18n::Message distributionTitle() const override {
    return OneMean::DistributionTitle();
  }
  void initParameters() override { OneMean::InitZIntervalParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::ZAuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }
  const char* estimateSymbol() const override {
    return OneMean::EstimateSymbol();
  }

 private:
  // Significance Test::OneMean
  int numberOfStatisticParameters() const override {
    return OneMean::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return OneMean::ZParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }
  void privateCompute() override { OneMean::ComputeZInterval(this); }

  double m_params[OneMean::k_numberOfParams];
};

}  // namespace Inference

#endif
