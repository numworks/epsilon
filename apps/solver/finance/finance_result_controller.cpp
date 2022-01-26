#include "finance_result_controller.h"
#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <poincare/print.h>
#include <assert.h>

using namespace Solver;

FinanceResultController::FinanceResultController(Escher::StackViewController * parentResponder, InterestData * data) :
      Escher::SelectableCellListPage<Escher::MessageTableCellWithMessageWithBuffer, k_numberOfResultCells>(parentResponder),
      m_messageView(KDFont::SmallFont, I18n::Message::CalculatedValues, KDContext::k_alignCenter, KDContext::k_alignCenter, Escher::Palette::GrayDark, Escher::Palette::WallScreen),
      m_contentView(&m_selectableTableView, this, &m_messageView),
      m_data(data) {
}

void FinanceResultController::didBecomeFirstResponder() {
  m_cells[0].setMessage(m_data->labelForParameter(m_data->getUnknown()));
  m_cells[0].setSubLabelMessage(m_data->sublabelForParameter(m_data->getUnknown()));
  double value = m_data->computeUnknownValue();
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
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

/* TODO Hugo : Improve Probability::CalculationController::updateTitle, which
 * may crash if text does not fit*/
const char * FinanceResultController::title() {
  // Try fitting the known parameters values in the title
  // We don't display the boolean parameter and the unknown one.
  uint8_t totalParameters = m_data->numberOfDoubleValues();
  // Using a minimal precision
  constexpr int precision = Poincare::Preferences::ShortNumberOfSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(precision);
  char valueBuffer[bufferSize];
  // Last char index before the text does not fit
  int charIndex = 0;
  uint8_t unknownIndex = m_data->getUnknown();
  bool unknownIndexIsLast = (unknownIndex == totalParameters - 1);
  for (uint8_t paramIndex = 0; paramIndex < totalParameters; paramIndex++) {
    if (paramIndex == unknownIndex) {
      // Avoid unknown parameter
      continue;
    }
    // Parameter's name
    const char * labelBuffer = I18n::translate(m_data->labelForParameter(paramIndex));
    // Parameter's value
    double value = m_data->getValue(paramIndex);
    Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, valueBuffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
    // Expected length
    int nextLength = charIndex + strlen(labelBuffer) + strlen("=") + strlen(valueBuffer);
    if (charIndex > 0) {
      // Add a space between chars
      nextLength += strlen(" ");
    }
    if (paramIndex != totalParameters - 1 - unknownIndexIsLast) {
      // If it is not the last known parameter, ensure a "..." fits after anyway
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
    charIndex += Poincare::Print::customPrintf(m_titleBuffer + charIndex, k_titleBufferSize - charIndex, "%s = %s", labelBuffer, valueBuffer);
    assert(charIndex < k_titleBufferSize);
  }
  m_titleBuffer[charIndex] = 0;
  return m_titleBuffer;
}
