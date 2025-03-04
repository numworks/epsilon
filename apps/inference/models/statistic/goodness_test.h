#ifndef INFERENCE_MODELS_STATISTIC_GOODNESS_TEST_H
#define INFERENCE_MODELS_STATISTIC_GOODNESS_TEST_H

#include "chi2_test.h"

namespace Inference {

class GoodnessTest final : public Chi2Test {
 public:
  GoodnessTest();
  constexpr PcrInference::CategoricalType categoricalType() const override {
    return PcrInference::CategoricalType::GoodnessOfFit;
  }
  int numberOfTestParameters() const override {
    return k_maxNumberOfRows * k_maxNumberOfColumns;
  }
  void setGraphTitle(char* buffer, size_t bufferSize) const override;
  void setResultTitle(char* buffer, size_t bufferSize,
                      bool resultIsTopPage) const override;

  // Inference
  bool validateInputs(int pageIndex = 0) override;
  void compute() override;

  // Table
  void recomputeData() override;
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; };
  int maxNumberOfRows() const override { return k_maxNumberOfRows; };

  // Chi2Test
  // Returns the contribution for column = 2
  double valueAtPosition(int row, int column) const override;

  // GoodnessTest
  int indexOfDegreeOfFreedom() const { return indexOfThreshold() + 1; }
  void setDegreeOfFreedom(double degreeOfFreedom) {
    m_degreesOfFreedom = degreeOfFreedom;
  }
  /* Return the DegreesOfFreedom computed from the numberOfValuesPairs. Actual
   * statistic's degree of freedom may differ because it can be overridden by
   * the user. */
  int computeDegreesOfFreedom() { return numberOfValuePairs() - 1; }

  constexpr static int k_maxNumberOfColumns = 2;

  // Chi2Test
  int numberOfValuePairs() const override;

 private:
  // Largest number of rows such that HomogeneityTest still takes more space.
  constexpr static int k_maxNumberOfRows = 90;
  /* It is not usefull to be able to set a degree of freedom much bigger than
   * the number of rows anyway. */
  constexpr static int k_maxDegreeOfFreedom = 10000;

  // Statistic
  int numberOfInferenceResults() const override { return 2; }
  void setParameterAtIndex(double p, int i) override;
  bool authorizedParameterAtIndex(double p, int i) const override;

  // Chi2Test
  double expectedValue(int index) const override;
  double observedValue(int index) const override;

  double* parametersArray() override { return m_input; }
  void setExpectedValue(int index, double value);
  void setObservedValue(int index, double value);
  Index2D initialDimensions() const override {
    return Index2D{.row = 1, .col = 2};
  }

  double m_input[k_maxNumberOfRows * k_maxNumberOfColumns];
};

}  // namespace Inference

#endif
