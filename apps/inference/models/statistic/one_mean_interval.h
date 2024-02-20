#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_INTERVAL_H

#include "interval.h"
#include "one_mean_statistic.h"

namespace Inference {

class OneMeanInterval : public Interval, public OneMeanStatistic {
 public:
  using OneMeanStatistic::OneMeanStatistic;

  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  I18n::Message title() const override { return OneMean::Title(oneMeanType()); }

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
  void initParameters() override {
    OneMean::InitIntervalParameters(oneMeanType(), this);
  }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::AuthorizedParameterAtIndex(oneMeanType(), i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParamaterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }
  bool validateInputs() override { return parametersAreValid(); }

  const char* estimateSymbol() const override {
    return OneMean::EstimateSymbol();
  }

 private:
  OneMean::Type oneMeanType() const {
    return OneMeanStatistic::OneMeanType(this);
  }
  void reinitParameters() override { initParameters(); }

  // Significance Test:: OneMean
  int numberOfStatisticParameters() const override {
    return OneMean::NumberOfParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return OneMean::ParameterRepresentationAtIndex(oneMeanType(), i);
  }
  double* parametersArray() override { return m_params; }

  void privateCompute() override {
    OneMean::ComputeInterval(oneMeanType(), this);
  }
};

class OneMeanTInterval : public OneMeanInterval {
 public:
  using OneMeanInterval::OneMeanInterval;

  DistributionType distributionType() const override {
    return DistributionType::T;
  }
};

class OneMeanZInterval : public OneMeanInterval {
 public:
  using OneMeanInterval::OneMeanInterval;

  DistributionType distributionType() const override {
    return DistributionType::Z;
  }
};

}  // namespace Inference

#endif
