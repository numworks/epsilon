#include "list_parameter_controller.h"
#include "list_controller.h"
#include "type_helper.h"
#include "../app.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  switch (type) {
  case 0:
    return &m_typeCell;
  case 1:
    return &m_renameCell;
  default:
    return Shared::ListParameterController::reusableCell(index, type - 2);
  }
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  Shared::ListParameterController::willDisplayCellForIndex(cell, index);
  if (cell == &m_typeCell && !m_record.isNull()) {
    App * myApp = App::app();
    assert(!m_record.isNull());
    Shared::ExpiringPointer<Shared::CartesianFunction> function = myApp->functionStore()->modelForRecord(m_record);
    m_typeCell.setMessage(I18n::Message::CurveType);
    int row = static_cast<int>(function->plotType());
    m_typeCell.setSubtitle(PlotTypeHelper::Message(row));
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
    renameFunction();
    return true;
  default:
    return Shared::ListParameterController::handleEnterOnRow(rowIndex - 2);
  }
}

void ListParameterController::renameFunction() {
  // Set editing true on function title
  StackViewController * stack = (StackViewController *)(parentResponder());
  stack->pop();
  m_listController->renameSelectedFunction();
}

}
