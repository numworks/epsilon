#include "goodness_test.h"

#include <inference/statistic/chi_square/results_goodness_table_cell.h>
#include <poincare/print.h>
#include <string.h>

#include "inference/models/statistic/chi2_test.h"

namespace Inference {

GoodnessTest::Column GoodnessTest::ColumnIndexForDataType(DataType type) {
  if (type == DataType::Observed) {
    return Column::Observed;
  }
  if (type == DataType::Expected) {
    return Column::Expected;
  }
  assert(type == DataType::Contribution);
  return Column::Contribution;
}

GoodnessTest::GoodnessTest() {
  invalidateNumberOfRows();
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    m_input[Column::Observed][i] = k_undefinedValue;
    m_input[Column::Expected][i] = k_undefinedValue;
  }
  init();
}

void GoodnessTest::init() {
  m_contributions = Poincare::FloatList<double>::Builder();
  for (int j = 0; j < k_maxNumberOfRows; j++) {
    m_contributions.addValueAtIndex(k_undefinedValue, j);
  }
}

void GoodnessTest::tidy() { m_contributions = Poincare::FloatList<double>(); }

void GoodnessTest::setGraphTitle(char* buffer, size_t bufferSize) const {
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "df=%*.*ed %s=%*.*ed %s=%*.*ed %s=%*.*ed",
      degreeOfFreedom(), Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      I18n::translate(I18n::Message::GreekAlpha), threshold(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      criticalValueSymbol(), testCriticalValue(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits,
      I18n::translate(I18n::Message::PValue), pValue(),
      Poincare::Preferences::PrintFloatMode::Decimal,
      Poincare::Preferences::ShortNumberOfSignificantDigits);
}

void GoodnessTest::setResultTitle(char* buffer, size_t bufferSize,
                                  bool resultIsTopPage) const {
  strlcpy(buffer, I18n::translate(I18n::Message::CalculatedValues), bufferSize);
}

double GoodnessTest::parameterAtIndex(int i) const {
  if (i == indexOfDegreesOfFreedom()) {
    return degreeOfFreedom();
  }
  return Chi2Test::parameterAtIndex(i);
}
void GoodnessTest::setParameterAtIndex(double f, int i) {
  if (i == indexOfDegreesOfFreedom()) {
    setDegreeOfFreedom(f);
    return;
  }
  Chi2Test::setParameterAtIndex(f, i);
}
bool GoodnessTest::authorizedParameterAtIndex(double p, int i) const {
  if (i == indexOfDegreesOfFreedom()) {
    return SignificanceTest::Chi2::IsDegreesOfFreedomValid(p);
  }
  return Chi2Test::authorizedParameterAtIndex(p, i);
}

bool GoodnessTest::validateInputs(int pageIndex) {
  return SignificanceTest::Chi2::AreGoodnessInputsValid(&m_observedValuesData,
                                                        &m_expectedValuesData);
}

int GoodnessTest::numberOfDataRows() const {
  if (m_numberOfDataRows < 0) {
    computeNumberOfRows();
  }
  return m_numberOfDataRows;
}

double GoodnessTest::dataValueAtLocation(DataType type, int col,
                                         int row) const {
  assert(col == 0 && row < k_maxNumberOfRows);
  return valueAtPosition(row, ColumnIndexForDataType(type));
}

void GoodnessTest::setDataValueAtLocation(DataType type, double value, int col,
                                          int row) {
  assert(col == 0 && row < k_maxNumberOfRows);
  return setValueAtPosition(value, row, ColumnIndexForDataType(type));
}

double GoodnessTest::valueAtPosition(int row, int column) const {
  if (column == Column::Contribution) {
    return m_contributions.valueAtIndex(row);
  }
  assert(column < k_numberOfInputColumns && row < k_maxNumberOfRows);
  return m_input[column][row];
}

void GoodnessTest::setValueAtPosition(double value, int row, int column) {
  if (column == Column::Contribution) {
    m_contributions.replaceValueAtIndex(value, row);
    return;
  }
  invalidateNumberOfRows();
  assert(column < k_numberOfInputColumns && row < k_maxNumberOfRows);
  m_input[column][row] = value;
}

bool GoodnessTest::authorizedValueAtPosition(double p, int row,
                                             int column) const {
  if (column == Column::Observed) {
    return SignificanceTest::Chi2::IsObservedValueValid(p);
  }
  assert(column == Column::Expected);
  return SignificanceTest::Chi2::IsExpectedValueValid(p);
}

void GoodnessTest::recomputeData() {
  // Delete empty rows
  int j = 0;
  int lastNonEmptyRow = -1;
  for (int i = 0; i < k_maxNumberOfRows; i++) {
    double expected = valueAtPosition(i, Column::Expected);
    double observed = valueAtPosition(i, Column::Observed);
    if (!(std::isnan(expected) && std::isnan(observed))) {
      if (i != j) {
        setValueAtPosition(expected, i, Column::Expected);
        setValueAtPosition(observed, i, Column::Observed);
      }
      j++;
      lastNonEmptyRow = i;
    }
  }
  while (j <= lastNonEmptyRow) {
    setValueAtPosition(k_undefinedValue, j, Column::Expected);
    setValueAtPosition(k_undefinedValue, j, Column::Observed);
    j++;
  }
  invalidateNumberOfRows();
}

int GoodnessTest::computeDegreesOfFreedom() {
  return SignificanceTest::Chi2::ComputeDegreesOfFreedom(categoricalType(),
                                                         &m_observedValuesData);
}

void GoodnessTest::computeNumberOfRows() const {
  Index2D dimensions = computeInnerDimensions();
  m_numberOfDataRows = dimensions.row;
}

}  // namespace Inference
