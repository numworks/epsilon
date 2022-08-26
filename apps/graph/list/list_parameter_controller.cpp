#include "list_parameter_controller.h"
#include "list_controller.h"
#include "../../shared/poincare_helpers.h"
#include "../app.h"
#include <escher/metric.h>
#include <poincare/print.h>
#include <assert.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate) :
  Shared::ListParameterController(parentResponder, functionColorMessage, deleteFunctionMessage),
  m_detailsCell(I18n::Message::Details),
  m_detailsParameterController(this),
  m_domainParameterController(nullptr, inputEventHandlerDelegate)
{
}

HighlightCell * ListParameterController::cell(int index) {
  assert(0 <= index && index < numberOfRows());
  index += !displayDetails() + !displayDomain(); // Skip hidden cells
  HighlightCell * const cells[] = {&m_detailsCell, &m_functionDomainCell, &m_enableCell, &m_colorCell, &m_deleteCell};
  return cells[index];
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  Shared::ListParameterController::setRecord(record);
  /* Set controllers' record here because we need to know which ones should be
   * displayed. */
  m_detailsParameterController.setRecord(m_record);
  m_domainParameterController.setRecord(m_record);
}

const char * intervalBracket(double value, bool opening) {
  if (std::isinf(value)) {
    return GlobalPreferences::sharedGlobalPreferences()->openIntervalChar(opening);
  }
  return opening ? "[" : "]";
}

int writeInterval(char * buffer, int bufferSize, double min, double max, int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  return Poincare::Print::CustomPrintf(buffer, bufferSize, "%s%*.*ed,%*.*ed%s",
    intervalBracket(min, true),
    min, mode, numberOfSignificantDigits,
    max, mode, numberOfSignificantDigits,
    intervalBracket(max, false));
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if ((cell == &m_detailsCell || cell == &m_functionDomainCell) && !m_record.isNull()) {
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<ContinuousFunction> function = myApp->functionStore()->modelForRecord(m_record);
    if (cell == &m_detailsCell) {
      m_detailsCell.setSubtitle(function->plotTypeMessage());
    } else {
      assert(cell == &m_functionDomainCell);
      m_functionDomainCell.setMessage(I18n::Message::FunctionDomain);
      double min = function->tMin();
      double max = function->tMax();
      constexpr int bufferSize = BufferTextView::k_maxNumberOfChar;
      char buffer[bufferSize];
      writeInterval(buffer, bufferSize, min, max, Preferences::VeryShortNumberOfSignificantDigits, Preferences::sharedPreferences()->displayMode());
      // Cell's layout will adapt to fit the subLabel.
      m_functionDomainCell.setSubLabelText(buffer);
    }
  }
}

void ListParameterController::detailsPressed() {
  static_cast<StackViewController *>(parentResponder())->push(&m_detailsParameterController);
}

void ListParameterController::functionDomainPressed() {
  static_cast<StackViewController *>(parentResponder())->push(&m_domainParameterController);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell * cell = selectedCell();
  if (cell == &m_detailsCell) {
    return m_detailsCell.handleEvent(event, this, &ListParameterController::detailsPressed);
  }
  if (cell == &m_functionDomainCell) {
    return m_functionDomainCell.handleEvent(event, this, &ListParameterController::functionDomainPressed);
  }
  return Shared::ListParameterController::handleEvent(event);
}

}
