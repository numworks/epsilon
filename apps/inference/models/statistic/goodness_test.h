#ifndef INFERENCE_MODELS_STATISTIC_GOODNESS_TEST_H
#define INFERENCE_MODELS_STATISTIC_GOODNESS_TEST_H

#include "chi2_test.h"
#include "poincare/old/float_list.h"

namespace Inference {

class GoodnessTest final : public Chi2Test {
 public:
  enum Column { Observed, Expected, Contribution };
  constexpr static int k_numberOfInputColumns = 2;
  static Column ColumnIndexForDataType(DataType type);

  GoodnessTest();
  void init() override;
  void tidy() override;

  constexpr CategoricalType categoricalType() const override {
    return CategoricalType::GoodnessOfFit;
  }

  void setGraphTitle(char* buffer, size_t bufferSize) const override;
  void setResultTitle(char* buffer, size_t bufferSize,
                      bool resultIsTopPage) const override;

  // Inference
  // +1 for degrees of freedom
  int numberOfParameters() override {
    return Chi2Test::numberOfParameters() + 1;
  }
  double parameterAtIndex(int i) const override;
  void setParameterAtIndex(double f, int i) override;
  bool authorizedParameterAtIndex(double p, int i) const override;
  int indexOfDegreesOfFreedom() const { return indexOfThreshold() + 1; }

  // Chi2
  int numberOfDataRows() const override;
  int numberOfDataColumns() const override { return 1; }
  double dataValueAtLocation(DataType type, int col, int row) const override;
  void setDataValueAtLocation(DataType type, double value, int col,
                              int row) override;

  // Test
  bool validateInputs(int pageIndex = 0) override;

  // Table
  void recomputeData() override;
  int maxNumberOfColumns() const override { return k_numberOfInputColumns; };
  int maxNumberOfRows() const override { return k_maxNumberOfRows; };
  // Returns the contribution for column = 2
  double valueAtPosition(int row, int column) const override;
  void setValueAtPosition(double value, int row, int column) override;
  bool authorizedValueAtPosition(double p, int row, int column) const override;

  // GoodnessTest
  void setDegreeOfFreedom(double degreeOfFreedom) {
    m_degreesOfFreedom = degreeOfFreedom;
  }
  /* Return the DegreesOfFreedom computed from the numberOfValuesPairs. Actual
   * statistic's degree of freedom may differ because it can be overridden by
   * the user. */
  int computeDegreesOfFreedom();
  bool isDegreesOfFreedomAuthorized(double p) const {
    return SignificanceTest::Chi2::IsDegreesOfFreedomValid(p);
  }

  // Chi2Test

 private:
  // Largest number of rows such that HomogeneityTest still takes more space.
  constexpr static int k_maxNumberOfRows = 90;
  /* It is not usefull to be able to set a degree of freedom much bigger than
   * the number of rows anyway. */
  constexpr static int k_maxDegreeOfFreedom = 10000;

  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = k_numberOfInputColumns};
  }
  void invalidateNumberOfRows() const { m_numberOfDataRows = -1; }
  void computeNumberOfRows() const;

  std::array<double, k_maxNumberOfRows> m_input[k_numberOfInputColumns];
  Poincare::FloatList<double> m_contributions;
  mutable int m_numberOfDataRows;
};

}  // namespace Inference

#endif
