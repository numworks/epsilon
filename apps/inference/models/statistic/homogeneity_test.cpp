#include "homogeneity_test.h"

#include <assert.h>
#include <poincare/print.h>

namespace Inference {

HomogeneityTest::HomogeneityTest() {
  invalidateDataDimensions();
  for (int col = 0; col < k_maxNumberOfColumns; col++) {
    for (int row = 0; row < k_maxNumberOfRows; row++) {
      m_input[col][row] = k_undefinedValue;
    }
  }
  init();
}

void HomogeneityTest::init() {
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_expectedValues[i] = Poincare::FloatList<double>::Builder();
    m_contributions[i] = Poincare::FloatList<double>::Builder();
    for (int j = 0; j < k_maxNumberOfRows; j++) {
      m_expectedValues[i].addValueAtIndex(k_undefinedValue, j);
      m_contributions[i].addValueAtIndex(k_undefinedValue, j);
    }
  }
}

void HomogeneityTest::tidy() {
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_expectedValues[i] = Poincare::FloatList<double>();
    m_contributions[i] = Poincare::FloatList<double>();
  }
}

void HomogeneityTest::setGraphTitle(char* buffer, size_t bufferSize) const {
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "%s=%*.*ed %s=%*.*ed %s=%*.*ed",
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

int HomogeneityTest::numberOfDataRows() const {
  if (m_numberOfDataRows < 0) {
    computeDataDimensions();
  }
  return m_numberOfDataRows;
}

int HomogeneityTest::numberOfDataColumns() const {
  if (m_numberOfDataColumns < 0) {
    computeDataDimensions();
  }
  return m_numberOfDataColumns;
}

double HomogeneityTest::dataValueAtLocation(DataType type, int col,
                                            int row) const {
  assert(col < k_maxNumberOfColumns && row < k_maxNumberOfRows);
  if (type == DataType::Observed) {
    return m_input[col][row];
  }
  if (type == DataType::Expected) {
    return m_expectedValues[col].valueAtIndex(row);
  }
  assert(type == DataType::Contribution);
  return m_contributions[col].valueAtIndex(row);
}

void HomogeneityTest::setDataValueAtLocation(DataType type, double value,
                                             int col, int row) {
  assert(col < k_maxNumberOfColumns && row < k_maxNumberOfRows);
  if (type == DataType::Observed) {
    m_input[col][row] = value;
    return;
  }
  if (type == DataType::Expected) {
    m_expectedValues[col].replaceValueAtIndex(value, row);
    return;
  }
  assert(type == DataType::Contribution);
  m_contributions[col].replaceValueAtIndex(value, row);
}

bool HomogeneityTest::validateInputs(int pageIndex) {
  return SignificanceTest::Chi2::AreHomogeneityInputsValid(
      &m_observedValuesData);
}

void HomogeneityTest::compute() {
  computeDataDimensions();
  computeExpectedValues();
  m_degreesOfFreedom = SignificanceTest::Chi2::ComputeDegreesOfFreedom(
      categoricalType(), &m_observedValuesData);
  Chi2Test::compute();
}

bool HomogeneityTest::authorizedValueAtPosition(double p, int row,
                                                int column) const {
  return SignificanceTest::Chi2::IsObservedValueValid(p);
}

bool HomogeneityTest::deleteValueAtPosition(int row, int column) {
  if (Chi2Test::deleteValueAtPosition(row, column)) {
    return true;
  }
  for (int j = 0; j < k_maxNumberOfRows; j++) {
    if (j != row && !std::isnan(valueAtPosition(j, column))) {
      // There is another non deleted value in this column
      return false;
    }
  }
  return true;
}

void HomogeneityTest::recomputeData() {
  // Remove empty rows / columns
  Index2D dimensions = computeInnerDimensions();
  // Start with rows
  int i = 0, j = 0;
  while (i < dimensions.row) {
    bool rowIsEmpty = true;
    for (int col = 0; col < dimensions.col; col++) {
      if (!std::isnan(valueAtPosition(i, col))) {
        rowIsEmpty = false;
        break;
      }
    }
    if (!rowIsEmpty) {
      if (i != j) {
        // Copy row from i to j
        for (int col = 0; col < dimensions.col; col++) {
          setValueAtPosition(valueAtPosition(i, col), j, col);
          setValueAtPosition(k_undefinedValue, i, col);
        }
      }
      j++;
    }
    i++;
  }

  // Columns
  i = 0, j = 0;
  while (i < dimensions.col) {
    bool colIsEmpty = true;
    for (int row = 0; row < dimensions.row; row++) {
      if (!std::isnan(valueAtPosition(row, i))) {
        colIsEmpty = false;
        break;
      }
    }
    if (!colIsEmpty) {
      if (i != j) {
        for (int row = 0; row < dimensions.row; row++) {
          setValueAtPosition(valueAtPosition(row, i), row, j);
          setValueAtPosition(k_undefinedValue, row, i);
        }
      }
      j++;
    }
    i++;
  }
  invalidateDataDimensions();
}

void HomogeneityTest::invalidateDataDimensions() const {
  m_numberOfDataRows = -1;
  m_numberOfDataColumns = -1;
}

void HomogeneityTest::computeDataDimensions() const {
  Index2D dimensions = computeInnerDimensions();
  m_numberOfDataRows = dimensions.row;
  m_numberOfDataColumns = dimensions.col;
}

void HomogeneityTest::computeExpectedValues() {
  SignificanceTest::Chi2::FillHomogeneityExpectedValues(&m_observedValuesData,
                                                        &m_expectedValuesData);
}

}  // namespace Inference
