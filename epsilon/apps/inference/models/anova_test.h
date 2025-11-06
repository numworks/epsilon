#pragma once

#include <omg/vector.h>

#include "input_table.h"
#include "significance_test.h"

namespace Inference {

class ANOVATest : public SignificanceTest, public InputTable {
 public:
  static constexpr size_t k_maxNumberOfGroups = 6;
  static constexpr size_t k_maxNumberOfValuesPerGroup = 12;

  constexpr static int k_maxNumberOfColumns = k_maxNumberOfGroups;
  constexpr static int k_maxNumberOfRows = k_maxNumberOfValuesPerGroup;

  constexpr TestType testType() const override { return TestType::ANOVA; }
  constexpr StatisticType statisticType() const override {
    return StatisticType::F;
  }

  void compute() override;

  // InputTable
  InputTable* table() override { return this; }

  uint8_t numberOfSeries() const override { return 0; }

  void setValueAtPosition(double value, int row, int column) override;

  double valueAtPosition(int row, int column) const override;

  bool authorizedValueAtPosition(double p, int row, int column) const override;

  void recomputeData() override {
    // TODO;
  }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return k_maxNumberOfRows; }

  Index2D initialDimensions() const override {
    return Index2D{.row = 2, .col = 3};
  }

  int numberOfGroups() const { return m_groups.size(); }

  using GroupValues = Poincare::Inference::SignificanceTest::FTest::Values;

  void setValues(std::span<const GroupValues> data) {
    m_groups.clear();
    for (const GroupValues& group : data) {
      m_groups.push(group);
    }
  }

  using Results =
      Poincare::Inference::SignificanceTest::FTest::StatisticResults;
  using CalculatedValues =
      Poincare::Inference::SignificanceTest::FTest::CalculatedValues;

  const Results& results() const { return m_results; }

 private:
  // Inference
  double* parametersArray() override { return nullptr; }
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

  static_assert(
      k_maxNumberOfGroups <=
      Poincare::Inference::SignificanceTest::FTest::k_maxNumberOfGroups);
  static_assert(k_maxNumberOfValuesPerGroup <= GroupValues{}.capacity());

  OMG::StaticVector<GroupValues, k_maxNumberOfGroups> m_groups;

  Results m_results;
};

}  // namespace Inference
