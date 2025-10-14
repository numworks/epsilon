#pragma once

#include <omg/vector.h>

#include "significance_test.h"

namespace Inference {

class ANOVATest : public SignificanceTest {
 public:
  constexpr TestType testType() const override { return TestType::ANOVA; }
  constexpr StatisticType statisticType() const override {
    return StatisticType::F;
  }

  void compute() override;

  using GroupValues = Poincare::Inference::SignificanceTest::FTest::Values;

  void setValues(std::span<const GroupValues> data) {
    m_groups.clear();
    for (const GroupValues& group : data) {
      m_groups.push(group);
    }
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

  static constexpr size_t k_maxNumberOfGroups = 6;
  static_assert(
      k_maxNumberOfGroups <=
      Poincare::Inference::SignificanceTest::FTest::k_maxNumberOfGroups);

  OMG::StaticVector<GroupValues, k_maxNumberOfGroups> m_groups;
};

}  // namespace Inference
