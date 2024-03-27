#ifndef INFERENCE_MODELS_STATISTIC_SLOPE_T_INTERVAL_H
#define INFERENCE_MODELS_STATISTIC_SLOPE_T_INTERVAL_H

#include "interval.h"
#include "slope_t_statistic.h"

namespace Inference {

class SlopeTInterval : public Interval, public SlopeTStatistic {
 public:
  SlopeTInterval(Shared::GlobalContext* context) : SlopeTStatistic(context) {}
  void init() override { DoublePairStore::initListsFromStorage(); }
  void tidy() override;
  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::Slope;
  }
  DistributionType distributionType() const override {
    return DistributionType::T;
  }
  I18n::Message title() const override { return SlopeTStatistic::title(); }

  // Inference
  bool authorizedParameterAtPosition(double p, int row,
                                     int column) const override {
    return Inference::authorizedParameterAtIndex(p,
                                                 index2DToIndex(row, column));
  }
  bool authorizedParameterAtIndex(double p, int i) const override {
    return Inference::authorizedParameterAtIndex(p, i) &&
           SlopeTStatistic::authorizedParameterAtIndex(p, i);
  }
  bool validateInputs() override { return SlopeTStatistic::validateInputs(); }

  // Distribution: t
  const char* estimateSymbol() const override { return "b"; }
  Poincare::Layout estimateLayout() const override;
  I18n::Message estimateDescription() override {
    return I18n::Message::SampleSlope;
  }

 private:
  // Significance Test: Slope
  int numberOfStatisticParameters() const override {
    return numberOfTableParameters();
  }
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;

  // Inference
  double* parametersArray() override {
    assert(false);
    return nullptr;
  }

  void privateCompute() override;
};

}  // namespace Inference

#endif
