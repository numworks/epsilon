#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_INTERVAL_H

#include "interval.h"
#include "one_mean_statistic.h"

namespace Inference {

class OneMeanInterval : public Interval, public OneMeanStatistic {
 public:
  using OneMeanStatistic::OneMeanStatistic;

  void init() override { initDatasetsIfSeries(); }
  void tidy() override { tidyDatasets(); }

  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::OneMean;
  }
  I18n::Message title() const override {
    return OneMean::Title(oneMeanType(this));
  }

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
    OneMean::InitIntervalParameters(oneMeanType(this), this);
  }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           OneMean::AuthorizedParameterAtIndex(oneMeanType(this), i, p);
  }
  void setParameterAtIndex(double p, int index) override {
    p = OneMean::ProcessParameterForIndex(p, index);
    Interval::setParameterAtIndex(p, index);
  }
  bool validateInputs(int pageIndex) override {
    return parametersAreValid(this, pageIndex);
  }
  int numberOfResults() const override {
    return numberOfResultsAndComputedParameters(this,
                                                Interval::numberOfResults());
  }
  int secondResultSectionStart() const override {
    return numberOfStatisticParameters();
  }
  void resultAtIndex(int index, double* value, Poincare::Layout* message,
                     I18n::Message* subMessage, int* precision) override {
    if (!computedParameterAtIndex(&index, this, value, message, subMessage,
                                  precision)) {
      Interval::resultAtIndex(index, value, message, subMessage, precision);
    }
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
    return OneMean::ParameterRepresentationAtIndex(oneMeanType(this), i);
  }
  double* parametersArray() override { return m_params; }

  void privateCompute() override {
    syncParametersWithStore(this);
    OneMean::ComputeInterval(oneMeanType(this), this);
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
