#ifndef INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_TWO_PROPORTIONS_Z_INTERVAL_H

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

  const char* estimateSymbol() const override {
    return TwoProportions::EstimateSymbol();
  }
  Poincare::Layout estimateLayout() const override {
    return TwoProportions::EstimateLayout(&m_estimateLayout);
  }
  I18n::Message estimateDescription() override {
    return TwoProportions::EstimateDescription();
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

  void privateCompute() override { TwoProportions::ComputeInterval(this); }

  double m_params[TwoProportions::k_numberOfParams];
};

}  // namespace Inference

#endif
