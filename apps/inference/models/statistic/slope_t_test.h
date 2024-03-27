#ifndef INFERENCE_MODELS_STATISTIC_SLOPE_T_TEST_H
#define INFERENCE_MODELS_STATISTIC_SLOPE_T_TEST_H

#include "slope_t_statistic.h"
#include "test.h"

namespace Inference {

class SlopeTTest : public Test, public SlopeTStatistic {
 public:
  SlopeTTest(Shared::GlobalContext* context) : SlopeTStatistic(context) {}
  void init() override { DoublePairStore::initListsFromStorage(); }
  void tidy() override { DoublePairStore::tidy(); }
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

  // Significance Test: Slope
  const char* hypothesisSymbol() override { return "β"; }

  void compute() override;

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
};

}  // namespace Inference

#endif
