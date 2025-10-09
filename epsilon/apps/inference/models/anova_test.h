#pragma once

#include <poincare/statistics/inference.h>

#include "significance_test.h"

namespace Inference {

class ANOVATest : public SignificanceTest {
 public:
  constexpr TestType testType() const override { return TestType::ANOVA; }
  constexpr StatisticType statisticType() const override {
    return StatisticType::F;
  }

  void compute() override {
    // TODO
    m_testCriticalValue = 0;
    m_pValue = 0;
  }

 private:
  // Inference
  double* parametersArray() override {
    assert(false);
    return nullptr;
  }
  float computeXMax() const override {
    // TODO
    return 1;
  }
  float computeXMin() const override {
    // TODO
    return -1;
  }
  bool shouldForbidZoom(float alpha, float criticalValue) override {
    return false;
  }
};

}  // namespace Inference
