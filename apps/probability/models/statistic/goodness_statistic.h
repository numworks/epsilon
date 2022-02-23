#ifndef PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H

#include "chi2_statistic.h"

namespace Probability {

class GoodnessStatistic final : public Chi2Statistic {
public:
  GoodnessStatistic();
  int numberOfStatisticParameters() const override { return k_maxNumberOfRows * k_maxNumberOfColumns; }

  // Statistic
  void computeTest() override;
  bool validateInputs() override;
  bool hasDegreeOfFreedom() override { return false; };

  // Chi2Statistic
  void setParameterAtPosition(int row, int column, double p) override;
  double parameterAtPosition(int row, int column) override;
  bool isValidParameterAtPosition(int row, int column, double p) override;
  bool deleteParameterAtPosition(int row, int column) override;
  void recomputeData() override;
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; };
  int maxNumberOfRows() const override { return k_maxNumberOfRows; };

  int computeNumberOfRows();
private:
  constexpr static int k_maxNumberOfRows = 10;
  constexpr static int k_maxNumberOfColumns = 2;

  // Statistic
  bool isValidParamAtIndex(int i, double p) override;

  // Chi2Statistic
  double expectedValue(int index) override;
  double observedValue(int index) override;
  int numberOfValuePairs() override;

  double * paramArray() override { return m_input; }
  int computeDegreesOfFreedom() { return numberOfValuePairs() - 1; }
  void setExpectedValue(int index, double value);
  void setObservedValue(int index, double value);
  int locationToTableIndex(int row, int column);

  double m_input[k_maxNumberOfRows * k_maxNumberOfColumns];
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_GOODNESS_STATISTIC_H */
