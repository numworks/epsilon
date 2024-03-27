#ifndef INFERENCE_MODELS_STATISTIC_CHI2_TEST_H
#define INFERENCE_MODELS_STATISTIC_CHI2_TEST_H

#include <cmath>

#include "table.h"
#include "test.h"

namespace Inference {

class Chi2Test : public Test, public Table {
 public:
  Chi2Test();

  SignificanceTestType significanceTestType() const override {
    return SignificanceTestType::Categorical;
  }
  DistributionType distributionType() const override {
    return DistributionType::Chi2;
  }
  bool initializeCategoricalType(CategoricalType type);

  // Table
  void setParameterAtPosition(double value, int row, int column) override {
    assert(index2DToIndex(row, column) < numberOfStatisticParameters());
    setParameterAtIndex(value, index2DToIndex(row, column));
  }
  double parameterAtPosition(int row, int column) const override {
    return parameterAtIndex(index2DToIndex(row, column));
  }
  bool authorizedParameterAtPosition(double p, int row,
                                     int column) const override {
    return authorizedParameterAtIndex(p, index2DToIndex(row, column));
  }

  bool authorizedParameterAtIndex(double p, int i) const override;

  virtual int numberOfValuePairs() const = 0;

 protected:
  using Test::parameterAtIndex;  // Hidden

  // Instantiate unused abstract class
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override;

  // Chi2 specific
  virtual double expectedValue(int index) const = 0;
  virtual double observedValue(int index) const = 0;
  double computeContribution(int index) const;
  double computeChi2();

 private:
  // Inference
  float computeXMin() const override {
    return DistribChi2.xMin(m_degreesOfFreedom);
  }
  float computeXMax() const override {
    return DistribChi2.xMax(m_degreesOfFreedom);
  }
};

}  // namespace Inference

#endif
