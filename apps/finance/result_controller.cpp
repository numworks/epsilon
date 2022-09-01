#include "result_controller.h"
#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <poincare/print.h>
#include <assert.h>

using namespace Finance;

ResultController::ResultController(Escher::StackViewController * parentResponder, Data * data) :
      Escher::SelectableCellListPage<Escher::MessageTableCellWithMessageWithBuffer, k_numberOfResultCells, Escher::MemoizedListViewDataSource>(parentResponder),
      DataController(data),
      m_messageView(KDFont::Size::Small, I18n::Message::CalculatedValues, KDContext::k_alignCenter, KDContext::k_alignCenter, Escher::Palette::GrayDark, Escher::Palette::WallScreen),
      m_contentView(&m_selectableTableView, this, &m_messageView) {
}

void ResultController::didBecomeFirstResponder() {
  /* Build the result cell here because it only needs to be updated once this
   * controller become first responder. */
  cellAtIndex(0)->setMessage(interestData()->labelForParameter(interestData()->getUnknown()));
  cellAtIndex(0)->setSubLabelMessage(interestData()->sublabelForParameter(interestData()->getUnknown()));
  double value = interestData()->computeUnknownValue();
  constexpr int maxUserPrecision = Poincare::PrintFloat::k_numberOfStoredSignificantDigits;
  constexpr int bufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(maxUserPrecision);
  char buffer[bufferSize];
  int precision = Poincare::Preferences::sharedPreferences()->numberOfSignificantDigits();
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(value, buffer, bufferSize, precision, Poincare::Preferences::PrintFloatMode::Decimal);
  cellAtIndex(0)->setAccessoryText(buffer);
  resetMemoization(true);
  selectRow(-1);
  m_contentView.reload();
}

bool ResultController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    Escher::Clipboard::sharedClipboard()->store(cellAtIndex(0)->text());
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

const char * ResultController::title() {
  /* Try fitting the known parameters values in the title using a minimal
   * precision. Use "..." at the end if not all parameters fit. */
  constexpr int precision = Poincare::Preferences::ShortNumberOfSignificantDigits;
  constexpr Poincare::Preferences::PrintFloatMode printFloatMode = Poincare::Preferences::PrintFloatMode::Decimal;
  // At least "..." should fit in the title.
  assert(k_titleBufferSize > strlen("..."));
  const char * parameterTemplate = "%s=%*.*ed...";
  const char * lastKnownParameterTemplate = "%s=%*.*ed";
  // The boolean parameter isn't displayed
  uint8_t doubleParameters = interestData()->numberOfDoubleValues();
  uint8_t unknownParam = interestData()->getUnknown();
  bool unknownParamIsLast = (unknownParam == doubleParameters - 1);
  size_t length = 0;
  for (uint8_t param = 0; param < doubleParameters; param++) {
    if (param == unknownParam) {
      // The unknown parameter isn't displayed
      continue;
    }
    // Ensure "..." fits if it isn't the last known parameter
    bool lastKnownParameter = (param == doubleParameters - 1 - unknownParamIsLast);
    // Attempting the parameter insertion
    int parameterLength = Poincare::Print::safeCustomPrintf(
        m_titleBuffer + length, k_titleBufferSize - length,
        (lastKnownParameter ? lastKnownParameterTemplate : parameterTemplate),
        I18n::translate(interestData()->labelForParameter(param)),
        interestData()->getValue(param), printFloatMode, precision);
    if (length + parameterLength >= k_titleBufferSize) {
      // Text did not fit, insert "..." and overwite last " " if there is one
      if (length > strlen(" ")) {
        length -= strlen(" ");
      }
      length += Poincare::Print::customPrintf(m_titleBuffer + length, k_titleBufferSize - length, "...");
      break;
    }
    length += parameterLength;
    if (!lastKnownParameter) {
      // Text did fit, "..." isn't needed for now and can be replaced with " "
      length -= strlen("...");
      length += Poincare::Print::customPrintf(m_titleBuffer + length, k_titleBufferSize - length, " ");
    }
  }
  assert(length < k_titleBufferSize);
  m_titleBuffer[length] = 0;
  return m_titleBuffer;
}
