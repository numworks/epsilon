#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_T_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_T_INTERVAL_H

#include "interval.h"

namespace Inference {

class OneMeanTInterval : public Interval {
 public:
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  DistributionType distributionType() const override {
    return DistributionType::T;
  }
  I18n::Message title() const override { return OneMean::TTitle(); }

  // Significance Test: One Mean
  bool initializeDistribution(DistributionType distributionType) override {
    return OneMean::IntervalInitializeDistribution(this, distributionType);
  }
  int numberOfAvailableDistributions() const override {
    return OneMean::NumberOfAvailableDistributions();
  }
  I18n::Message distributionTitle() const override {
    return OneMean::DistributionTitle();
  }
  void initParameters() override { OneMean::InitTIntervalParameters(this); }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::TAuthorizedParameterAtIndex(i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }

  const char* estimateSymbol() const override {
    return OneMean::EstimateSymbol();
  }

 private:
  // Significance Test:: OneMean
  int numberOfStatisticParameters() const override {
    return OneMean::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return OneMean::TParameterRepresentationAtIndex(i);
  }
  double* parametersArray() override { return m_params; }

  void privateCompute() override { OneMean::ComputeTInterval(this); }

  double m_params[OneMean::k_numberOfParams];
};

}  // namespace Inference

#endif
