#include "list_parameter_controller.h"
#include "list_controller.h"
#include "type_helper.h"
#include "../../shared/poincare_helpers.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Graph {

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  switch (type) {
  case 0:
    return &m_typeCell;
  case 1:
    return &m_functionDomain;
  case 2:
    return &m_renameCell;
  default:
    return Shared::ListParameterController::reusableCell(index, type - 3);
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

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if ((cell == &m_typeCell || cell == &m_functionDomain) && !m_record.isNull()) {
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<Shared::CartesianFunction> function = myApp->functionStore()->modelForRecord(m_record);
    if (cell == &m_typeCell) {
      m_typeCell.setMessage(I18n::Message::CurveType);
      int row = static_cast<int>(function->plotType());
      m_typeCell.setSubtitle(PlotTypeHelper::Message(row));
    } else {
      assert(cell == &m_functionDomain);
      m_functionDomain.setMessage(I18n::Message::FunctionDomain);
      constexpr int bufferSize = BufferTextView::k_maxNumberOfChar;
      char buffer[bufferSize];
      int numberOfChar = PoincareHelpers::ConvertFloatToText<double>(function->tMin(), buffer, bufferSize, Preferences::VeryShortNumberOfSignificantDigits);
      numberOfChar += strlcpy(buffer+numberOfChar, "..", bufferSize-numberOfChar);
      numberOfChar += PoincareHelpers::ConvertFloatToText<double>(function->tMax(), buffer+numberOfChar, bufferSize-numberOfChar, Preferences::VeryShortNumberOfSignificantDigits);
      numberOfChar += strlcpy(buffer+numberOfChar, " ", bufferSize-numberOfChar);
      m_functionDomain.setAccessoryText(buffer);
    }
  }
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  switch (rowIndex) {
  case 0:
    m_typeParameterController.setRecord(m_record);
    stack->push(&m_typeParameterController);
    return true;
  case 1:
    m_domainParameterController.setRecord(m_record);
    stack->push(&m_domainParameterController);
    return true;
  case 2:
    renameFunction();
    return true;
  default:
    return Shared::ListParameterController::handleEnterOnRow(rowIndex - 3);
  }
}

void ListParameterController::renameFunction() {
  // Set editing true on function title
  StackViewController * stack = (StackViewController *)(parentResponder());
  stack->pop();
  m_listController->renameSelectedFunction();
}

}
