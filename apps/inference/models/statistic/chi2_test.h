#ifndef INFERENCE_MODELS_STATISTIC_CHI2_TEST_H
#define INFERENCE_MODELS_STATISTIC_CHI2_TEST_H

#include <poincare/statistics/inference.h>

#include "table.h"
#include "test.h"

namespace Inference {

class Chi2Test : public Test, public Table {
 public:
  Chi2Test()
      : m_observedValuesData(this),
        m_expectedValuesData(this),
        m_contributionsData(this) {}

  constexpr TestType testType() const override { return TestType::Chi2; }
  constexpr StatisticType statisticType() const override {
    return StatisticType::Chi2;
  }
  Table* table() override { return this; }

  enum class DataType { Observed, Expected, Contribution };

  virtual int numberOfDataColumns() const = 0;
  virtual int numberOfDataRows() const = 0;
  virtual double dataValueAtLocation(DataType type, int col, int row) const = 0;
  virtual void setDataValueAtLocation(DataType type, double value, int col,
                                      int row) = 0;

  // Table
  int numberOfSeries() const override { return 0; }

  void compute() override;

 protected:
  template <DataType T>
  class Chi2DataTable : public Poincare::DataTable {
   public:
    Chi2DataTable(Chi2Test* delegate) : m_delegate(delegate) {}
    int numberOfColumns() const override {
      return m_delegate->numberOfDataColumns();
    }
    int numberOfRows() const override { return m_delegate->numberOfDataRows(); }
    double get(int col, int row) const override {
      return m_delegate->dataValueAtLocation(T, col, row);
    }
    void set(double value, int col, int row) override {
      m_delegate->setDataValueAtLocation(T, value, col, row);
    }

   private:
    Chi2Test* m_delegate;
  };

  Chi2DataTable<DataType::Observed> m_observedValuesData;
  Chi2DataTable<DataType::Expected> m_expectedValuesData;
  Chi2DataTable<DataType::Contribution> m_contributionsData;

 private:
  void computeContributions();

  // Inference
  double* parametersArray() override { return nullptr; }
  float computeXMax() const override;
  float computeXMin() const override;
  bool shouldForbidZoom(float alpha, float criticalValue) override {
    return false;
  }
};

}  // namespace Inference

#endif
