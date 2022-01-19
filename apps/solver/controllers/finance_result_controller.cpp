#include "finance_result_controller.h"

#include <apps/i18n.h>
#include <assert.h>
#include <escher/container.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/print.h>
#include <escher/clipboard.h>

using namespace Solver;

FinanceResultController::FinanceResultController(Escher::StackViewController * parentResponder, FinanceData * data) :
      SelectableCellListPage(parentResponder), m_contentView(&m_selectableTableView, this, I18n::Message::CalculatedValues), m_data(data) {
}

void FinanceResultController::didBecomeFirstResponder() {
  double value;
  if (m_data->isSimpleInterest) {
    SimpleInterestParameter unknownParam = simpleInterestData()->getUnknown();
    m_cells[0].setMessage(SimpleInterestData::LabelForParameter(unknownParam));
    m_cells[0].setSubLabelMessage(SimpleInterestData::SublabelForParameter(unknownParam));
    value = simpleInterestData()->computeUnknownValue();
  } else {
    CompoundInterestParameter unknownParam = compoundInterestData()->getUnknown();
    m_cells[0].setMessage(CompoundInterestData::LabelForParameter(unknownParam));
    m_cells[0].setSubLabelMessage(CompoundInterestData::SublabelForParameter(unknownParam));
    value = compoundInterestData()->computeUnknownValue();
  }
  constexpr int precision = Poincare::Preferences::LargeNumberOfSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  char buffer[bufferSize];
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
  m_cells[0].setAccessoryText(buffer);
  resetMemoization(true);
  selectRow(-1);
  m_contentView.reload();
}

bool FinanceResultController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    Escher::Clipboard::sharedClipboard()->store(m_cells[0].text());
  } else if (event == Ion::Events::Left) {
    stackViewController()->pop();
  } else {
    return false;
  }
  return true;
}

/* TODO Hugo : Improve Probability::CalculationController::updateTitle, which
 * may crash if text does not fit*/
const char * FinanceResultController::title() {
  // Try fitting the known parameters values in the title
  // We don't display the boolean parameter and the unknown one.
  int totalParameters = (m_data->isSimpleInterest ? k_numberOfSimpleInterestParameters : k_numberOfCompoundInterestParameters) - 2;
  // Using a minimal precision
  constexpr int precision = Poincare::Preferences::ShortNumberOfSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  char valueBuffer[bufferSize];
  // Last char index before the text does not fit
  int charIndex = 0;
  int paramIndex = 0;
  int unknownIndex = m_data->isSimpleInterest ? static_cast<int>(simpleInterestData()->getUnknown()) : static_cast<int>(compoundInterestData()->getUnknown());
  for (int i = 0; i < totalParameters; i++) {
    if (paramIndex == unknownIndex) {
      // Avoid unknown parameter
      paramIndex += 1;
    }
    // Parameter's name
    I18n::Message label = m_data->isSimpleInterest ?
      SimpleInterestData::LabelForParameter(static_cast<SimpleInterestParameter>(paramIndex)) :
      CompoundInterestData::LabelForParameter(static_cast<CompoundInterestParameter>(paramIndex));
    const char * labelBuffer = I18n::translate(label);
    // Parameter's value
    double value = m_data->isSimpleInterest ?
      m_data->m_data.m_simpleInterestData.getValue(static_cast<SimpleInterestParameter>(paramIndex)) :
      m_data->m_data.m_compoundInterestData.getValue(static_cast<CompoundInterestParameter>(paramIndex));
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, valueBuffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
    // Expected length
    int nextLength = charIndex + strlen(labelBuffer) + strlen("=") + strlen(valueBuffer);
    if (charIndex > 0) {
      // Add a space between chars
      nextLength += strlen(" ");
    }
    if (paramIndex != totalParameters) {
      // If it is not the last parameter, ensure a "..." fits after anyway
      nextLength += strlen("...");
    }
    if (nextLength >= k_titleBufferSize) {
      // Text does not fit, insert "..."
      charIndex += strlcpy(m_titleBuffer + charIndex, "...", k_titleBufferSize - charIndex);
      break;
    }
    if (charIndex > 0) {
      // Insert a space in between parameters
      charIndex += strlcpy(m_titleBuffer + charIndex, " ", k_titleBufferSize - charIndex);
    }
    // Insert label, '=' and value
    charIndex += strlcpy(m_titleBuffer + charIndex, labelBuffer, k_titleBufferSize - charIndex);
    charIndex += strlcpy(m_titleBuffer + charIndex, "=", k_titleBufferSize - charIndex);
    charIndex += strlcpy(m_titleBuffer + charIndex, valueBuffer, k_titleBufferSize - charIndex);
    paramIndex += 1;
    assert(charIndex < k_titleBufferSize);
  }
  m_titleBuffer[charIndex] = 0;
  return m_titleBuffer;
}
