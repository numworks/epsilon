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
  m_typeCell(),
  m_typeParameterController(this),
  m_domainParameterController(nullptr, inputEventHandlerDelegate)
{
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  if (!displayDetails()) {
    index += 1;
  }
  switch (index) {
  case 0:
    return &m_typeCell;
  case 1:
    return &m_functionDomain;
  default:
    if (!displayDetails()) {
      index -= 1;
    }
    return Shared::ListParameterController::reusableCell(index, type);
  }
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (Shared::ListParameterController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::Right) {
    int selectedR = selectedRow();
    if (selectedR == 0 || selectedR == 1) {
      // Go in the submenu
      return handleEnterOnRow(selectedR);
    }
  }
  return false;
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  Shared::ListParameterController::setRecord(record);
  /* Set m_typeParameterController record here because we need record's
   * detailsNumberOfSections() to know if it should be displayed. */
  m_typeParameterController.setRecord(m_record);
}

char intervalBracket(double value, bool opening) {
  return std::isinf(value) == opening ? ']' : '[';
}

int writeInterval(char * buffer, int bufferSize, double min, double max, int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  return Poincare::Print::customPrintf(buffer, bufferSize, "%c%*.*ed,%*.*ed%c",
    intervalBracket(min, true),
    min, mode, numberOfSignificantDigits,
    max, mode, numberOfSignificantDigits,
    intervalBracket(max, false));
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if ((cell == &m_typeCell || cell == &m_functionDomain) && !m_record.isNull()) {
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<ContinuousFunction> function = myApp->functionStore()->modelForRecord(m_record);
    if (cell == &m_typeCell) {
      m_typeCell.setMessage(I18n::Message::Details);
      m_typeCell.setSubtitle(function->plotTypeMessage());
    } else {
      assert(cell == &m_functionDomain);
      m_functionDomain.setMessage(I18n::Message::FunctionDomain);
      double min = function->tMin();
      double max = function->tMax();
      constexpr int bufferSize = BufferTextView::k_maxNumberOfChar;
      char buffer[bufferSize];
      writeInterval(buffer, bufferSize, min, max, Preferences::VeryShortNumberOfSignificantDigits, Preferences::sharedPreferences()->displayMode());
      // Cell's layout will adapt to fit the subLabel.
      m_functionDomain.setSubLabelText(buffer);
    }
  }
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  if (!displayDetails()) {
    rowIndex += 1;
  }
  switch (rowIndex) {
  case 0:
    stack->push(&m_typeParameterController);
    return true;
  case 1:
    m_domainParameterController.setRecord(m_record);
    stack->push(&m_domainParameterController);
    return true;
  default:
    if (!displayDetails()) {
      rowIndex -= 1;
    }
    return Shared::ListParameterController::handleEnterOnRow(rowIndex);
  }
}

}
