#ifndef INFERENCE_MODELS_STATISTIC_CHI2_TEST_H
#define INFERENCE_MODELS_STATISTIC_CHI2_TEST_H

#include "table.h"
#include "test.h"

namespace Inference {

class Chi2Test : public Test, public Table {
 public:
  Chi2Test();

  constexpr PcrInference::TestType testType() const override {
    return PcrInference::TestType::Chi2;
  }
  constexpr PcrInference::StatisticType statisticType() const override {
    return PcrInference::StatisticType::Chi2;
  }
  Table* table() override { return this; }

  // Table
  int numberOfSeries() const override { return 0; }

  void setValueAtPosition(double value, int row, int column) override {
    assert(index2DToIndex(row, column) < numberOfTestParameters());
    setParameterAtIndex(value, index2DToIndex(row, column));
  }
  double valueAtPosition(int row, int column) const override {
    return parameterAtIndex(index2DToIndex(row, column));
  }
  bool authorizedValueAtPosition(double p, int row, int column) const override {
    return authorizedParameterAtIndex(p, index2DToIndex(row, column));
  }

  virtual int numberOfValuePairs() const = 0;

 protected:
  using Test::parameterAtIndex;  // Hidden

  // Chi2 specific
  virtual double expectedValue(int index) const = 0;
  virtual double observedValue(int index) const = 0;
  double computeContribution(int index) const;
  double computeChi2();

 private:
  // Inference
  float computeXMax() const override;
  float computeXMin() const override;
  bool shouldForbidZoom(float alpha, float criticalValue) override {
    return false;
  }
};

}  // namespace Inference

#endif
