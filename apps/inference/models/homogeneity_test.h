#ifndef INFERENCE_MODELS_STATISTIC_HOMOGENEITY_TEST_H
#define INFERENCE_MODELS_STATISTIC_HOMOGENEITY_TEST_H

#include "chi2_test.h"
#include "poincare/old/float_list.h"

namespace Inference {

class HomogeneityTest final : public Chi2Test {
 public:
  // Used in HomogeneityTableDataSource
  constexpr static int k_maxNumberOfColumns = 9;
  constexpr static int k_maxNumberOfRows = 9;

  HomogeneityTest();
  void init() override;
  void tidy() override;

  constexpr CategoricalType categoricalType() const override {
    return CategoricalType::Homogeneity;
  }
  void setGraphTitle(char* buffer, size_t bufferSize) const override;

  // Chi2Test
  int numberOfDataRows() const override;
  int numberOfDataColumns() const override;
  double dataValueAtLocation(DataType type, int col, int row) const override;
  void setDataValueAtLocation(DataType type, double value, int col,
                              int row) override;

  // Statistic
  bool validateInputs(int pageIndex = 0) override;
  void compute() override;

  // Table
  void recomputeData() override;
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; };
  int maxNumberOfRows() const override { return k_maxNumberOfRows; };
  double valueAtPosition(int row, int column) const override {
    return dataValueAtLocation(DataType::Observed, column, row);
  }
  void setValueAtPosition(double value, int row, int column) override {
    setDataValueAtLocation(DataType::Observed, value, column, row);
  }
  bool authorizedValueAtPosition(double p, int row, int column) const override;
  bool deleteValueAtPosition(int row, int column) override;

  // HomogeneityTest
  double total() { return m_observedValuesData.totalSum(); }
  double rowTotal(int row) { return m_observedValuesData.rowSum(row); }
  double columnTotal(int column) {
    return m_observedValuesData.columnSum(column);
  }

 private:
  Index2D initialDimensions() const override {
    return Index2D{.row = 2, .col = 2};
  }
  void invalidateDataDimensions() const;
  void computeDataDimensions() const;

  void computeExpectedValues();

  std::array<std::array<double, k_maxNumberOfRows>, k_maxNumberOfColumns>
      m_input;
  Poincare::FloatList<double> m_expectedValues[k_maxNumberOfColumns];
  Poincare::FloatList<double> m_contributions[k_maxNumberOfColumns];
  mutable int m_numberOfDataRows;
  mutable int m_numberOfDataColumns;
};

}  // namespace Inference

#endif
