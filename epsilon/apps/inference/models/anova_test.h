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

  // InferenceModel
  constexpr TestType testType() const override { return TestType::ANOVA; }
  constexpr StatisticType statisticType() const override {
    return StatisticType::F;
  }

  Poincare::Distribution::ParametersArray<double> distributionParameters()
      const override {
    return Poincare::Inference::DistributionParameters(
        statisticType(), m_results.between.degreesOfFreedom,
        m_results.within.degreesOfFreedom);
  }

  bool validateInputs(int /* pageIndex */) override;

  void compute() override;

  // InputTable
  InputTable* table() override { return this; }

  uint8_t numberOfSeries() const override { return 0; }

  void setValueAtPosition(double value, int row, int column) override;
  double valueAtPosition(int row, int column) const override;
  // Return true if the group data was changed
  bool deleteValueAtPosition(int row, int column) override;
  void deleteValuesInColumn(int column) override;
  bool authorizedValueAtPosition(double p, int row, int column) const override;

  void recomputeData() override {}

  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return k_maxNumberOfRows; }

  Index2D initialDimensions() const override {
    return Index2D{.row = 2, .col = 3};
  }

  enum class InputMode : bool { Values, Statistics };

  void setInputMode(InputMode mode) { m_inputMode = mode; }

  int numberOfGroups() const {
    return (m_inputMode == InputMode::Values) ? m_groups.size()
                                              : m_inputStatistics.size();
  }

  using StatisticsData =
      Poincare::Inference::SignificanceTest::FTest::StatisticsData;

  class InputStatisticsData {
   public:
    double parameter(int parameterIndex) const;

    double numberOfSamples() const {
      return m_statisticsData.numberOfSamples();
    }
    double mean() const { return m_statisticsData.mean(); }
    double sampleStdDeviation() const {
      return m_statisticsData.sampleStdDeviation();
    }

    void set(double value, int parameterIndex);

    void unsetStatisticParameter(int parameterIndex);

    bool isUnset() const {
      return std::isnan(m_statisticsData.numberOfSamples()) &&
             std::isnan(m_statisticsData.mean()) &&
             std::isnan(m_statisticsData.sampleStdDeviation());
    }

    const StatisticsData& statisticsData() const { return m_statisticsData; }

   private:
    StatisticsData m_statisticsData{k_undefinedValue, k_undefinedValue,
                                    k_undefinedValue};
  };

  using GroupValues = Poincare::Inference::SignificanceTest::FTest::Values;
  using GroupData = Poincare::Inference::SignificanceTest::FTest::GroupData;

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

  const GroupData& groupStatistics(int groupIndex) const {
    assert(groupIndex >= 0 && groupIndex < numberOfGroups());
    return m_groupData[groupIndex];
  }

 private:
  void setGroupValue(double value, int row, int column);
  double groupValue(int row, int column) const;
  bool deleteGroupValue(int row, int column);
  bool validateGroupInputs() const;

  void setStatisticParameter(double value, int parameterIndex, int groupIndex);
  double statisticParameter(int parameterIndex, int groupIndex) const;
  // Return true if the number of groups was changed
  bool deleteStatisticParameter(int parameterIndex, int groupIndex);
  bool validateStatisticInputs() const;

  void computeGroupData();

  // InferenceModel
  double* parametersArray() override { return nullptr; }

  float computeXMin() const override { return 0.f; }
  bool shouldForbidZoom(float alpha, float criticalValue) override {
    return false;
  }

  static_assert(
      k_maxNumberOfGroups <=
      Poincare::Inference::SignificanceTest::FTest::k_maxNumberOfGroups);
  static_assert(k_maxNumberOfValuesPerGroup <= GroupValues{}.capacity());

  // TODO: variant to share memory space between m_groups and m_groupData
  OMG::StaticVector<GroupValues, k_maxNumberOfGroups> m_groups;
  OMG::StaticVector<InputStatisticsData, k_maxNumberOfGroups> m_inputStatistics;

  OMG::StaticVector<GroupData, k_maxNumberOfGroups> m_groupData;
  Results m_results;

  InputMode m_inputMode = InputMode::Values;
};

}  // namespace Inference
