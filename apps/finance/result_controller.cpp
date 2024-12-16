#include "result_controller.h"

#include <apps/i18n.h>
#include <apps/shared/poincare_helpers.h>
#include <assert.h>
#include <escher/clipboard.h>
#include <escher/container.h>
#include <poincare/print.h>

#include "app.h"

namespace Finance {

ResultController::ResultController(Escher::StackViewController* parentResponder)
    : Escher::ListWithTopAndBottomController(parentResponder, &m_messageView),
      m_messageView(I18n::Message::CalculatedValues, k_messageFormat) {}

void ResultController::viewWillAppear() {
  /* Build the result cell here because it only needs to be updated once this
   * controller become first responder. */
  m_cell.label()->setMessage(App::GetInterestData()->labelForParameter(
      App::GetInterestData()->getUnknown()));
  m_cell.subLabel()->setMessage(App::GetInterestData()->sublabelForParameter(
      App::GetInterestData()->getUnknown()));
  double value = App::GetInterestData()->computeUnknownValue();
  constexpr int bufferSize = Escher::FloatBufferTextView<>::MaxTextSize();
  char buffer[bufferSize];
  int precision =
      Poincare::Preferences::SharedPreferences()->numberOfSignificantDigits();
  Shared::PoincareHelpers::ConvertFloatToTextWithDisplayMode<double>(
      value, buffer, bufferSize, precision,
      Poincare::Preferences::PrintFloatMode::Decimal);
  m_cell.accessory()->setText(buffer);
  selectRow(-1);
  m_selectableListView.reloadData(false);
  ViewController::viewWillAppear();
}

bool ResultController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    Escher::Clipboard::SharedClipboard()->storeText(m_cell.text());
    return true;
  }
  if (event == Ion::Events::Sto || event == Ion::Events::Var) {
    App::app()->storeLayout(Poincare::Layout::String(m_cell.text()));
    return true;
  }
  return popFromStackViewControllerOnLeftEvent(event);
}

const char* ResultController::title() const {
  /* Try fitting the known parameters values in the title using a minimal
   * precision. Use "..." at the end if not all parameters fit. */
  constexpr int precision =
      Poincare::Preferences::ShortNumberOfSignificantDigits;
  constexpr Poincare::Preferences::PrintFloatMode printFloatMode =
      Poincare::Preferences::PrintFloatMode::Decimal;
  // At least "..." should fit in the title.
  assert(k_titleBufferSize > strlen("..."));
  constexpr const char* parameterTemplate = "%s=%*.*ed...";
  constexpr const char* lastKnownParameterTemplate = "%s=%*.*ed";
  // The boolean parameter isn't displayed
  uint8_t doubleParameters = App::GetInterestData()->numberOfDoubleValues();
  uint8_t unknownParam = App::GetInterestData()->getUnknown();
  bool unknownParamIsLast = (unknownParam == doubleParameters - 1);
  size_t length = 0;
  for (uint8_t param = 0; param < doubleParameters; param++) {
    if (param == unknownParam) {
      // The unknown parameter isn't displayed
      continue;
    }
    // Ensure "..." fits if it isn't the last known parameter
    bool lastKnownParameter =
        (param == doubleParameters - 1 - unknownParamIsLast);
    // Attempting the parameter insertion
    int parameterLength = Poincare::Print::UnsafeCustomPrintf(
        m_titleBuffer + length, k_titleBufferSize - length,
        (lastKnownParameter ? lastKnownParameterTemplate : parameterTemplate),
        I18n::translate(App::GetInterestData()->labelForParameter(param)),
        App::GetInterestData()->getValue(param), printFloatMode, precision);
    if (length + parameterLength >= k_titleBufferSize) {
      // Text did not fit, insert "..." and overwite last " " if there is one
      if (length > strlen(" ")) {
        length -= strlen(" ");
      }
      length += Poincare::Print::CustomPrintf(
          m_titleBuffer + length, k_titleBufferSize - length, "...");
      break;
    }
    length += parameterLength;
    if (!lastKnownParameter) {
      // Text did fit, "..." isn't needed for now and can be replaced with " "
      length -= strlen("...");
      length += Poincare::Print::CustomPrintf(m_titleBuffer + length,
                                              k_titleBufferSize - length, " ");
    }
  }
  assert(length < k_titleBufferSize);
  m_titleBuffer[length] = 0;
  return m_titleBuffer;
}

}  // namespace Finance
