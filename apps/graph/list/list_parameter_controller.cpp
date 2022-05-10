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

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  switch (type) {
  case k_detailsCellType:
    assert(displayDetails());
    return &m_detailsCell;
  case k_domainCellType:
    assert(displayDomain());
    return &m_functionDomain;
  default:
    return Shared::ListParameterController::reusableCell(index, type);
  }
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  Shared::ListParameterController::setRecord(record);
  /* Set controllers' record here because we need to know which ones should be
   * displayed. */
  m_detailsParameterController.setRecord(m_record);
  m_domainParameterController.setRecord(m_record);
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
  if ((cell == &m_detailsCell || cell == &m_functionDomain) && !m_record.isNull()) {
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<ContinuousFunction> function = myApp->functionStore()->modelForRecord(m_record);
    if (cell == &m_detailsCell) {
      assert(typeAtIndex(index) == k_detailsCellType);
      m_detailsCell.setSubtitle(function->plotTypeMessage());
    } else {
      assert(cell == &m_functionDomain && typeAtIndex(index) == k_domainCellType);
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

int ListParameterController::typeAtIndex(int index) {
  if (displayDetails() && index == 0) {
    return k_detailsCellType;
  }
  if (displayDomain() && index == displayDetails()) {
    return k_domainCellType;
  }
  return Shared::ListParameterController::typeAtIndex(index);
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  int type = typeAtIndex(rowIndex);
  switch (type) {
    case k_detailsCellType:
    stack->push(&m_detailsParameterController);
    return true;
  case k_domainCellType:
    stack->push(&m_domainParameterController);
    return true;
  default:
    return Shared::ListParameterController::handleEnterOnRow(rowIndex);
  }
}

bool ListParameterController::rightEventIsEnterOnType(int type) {
  return type == k_detailsCellType || type == k_domainCellType || Shared::ListParameterController::rightEventIsEnterOnType(type);
}

}
