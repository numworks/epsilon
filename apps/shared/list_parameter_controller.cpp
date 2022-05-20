#include "list_parameter_controller.h"
#include "function_app.h"
#include "color_names.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  SelectableListViewController(parentResponder, tableDelegate),
  m_enableCell(I18n::Message::ActivateDeactivateListParamTitle, I18n::Message::ActivateDeactivateListParamDescription, true),
  m_deleteCell(deleteFunctionMessage),
  m_colorParameterController(this)
{}

const char * ListParameterController::title() {
  return I18n::translate(I18n::Message::FunctionOptions);
}

void ListParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ListParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  resetMemoization();
  m_selectableTableView.reloadData();
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_enableCell && !m_record.isNull()) {
    assert(typeAtIndex(index) == k_enableCellType);
    m_enableCell.setState(function()->isActive());
  }
  if (cell == &m_colorCell) {
    m_colorCell.setMessage(I18n::Message::Color);
    m_colorCell.setSubtitle(ColorNames::NameForColor(function()->color()));
  }
}

int ListParameterController::typeAtIndex(int index) {
  switch (sharedCellIndex(index)) {
  case 0:
    return k_enableCellType;
  case 1:
    return k_colorCellType;
  default:
    assert(sharedCellIndex(index) == 2);
    return k_deleteCellType;
  }
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectCellAtLocation(0, 0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && rightEventIsEnterOnType(typeAtIndex(selectedRow())))) {
    return handleEnterOnRow(selectedRow());
  }
  return false;
}

int ListParameterController::sharedCellIndex(int j) {
  // Shared::ListParameterController rows are always placed last
  assert(j >= numberOfRows() - k_numberOfSharedCells && j < numberOfRows());
  return j - (numberOfRows() - k_numberOfSharedCells);
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  assert(type >= 0);
  assert(type < k_numberOfSharedCells);
  HighlightCell * const cells[k_numberOfSharedCells] = {&m_enableCell, &m_colorCell, &m_deleteCell};
  return cells[type];
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  switch (typeAtIndex(rowIndex)) {
    case k_enableCellType:
      function()->setActive(!function()->isActive());
      resetMemoization();
      m_selectableTableView.reloadData();
      return true;
    case k_colorCellType:
      m_colorParameterController.setRecord(m_record);
      stack->push(&m_colorParameterController);
      return true;
    case k_deleteCellType:
      {
        assert(functionStore()->numberOfModels() > 0);
        m_selectableTableView.deselectTable();
        functionStore()->removeModel(m_record);
        setRecord(Ion::Storage::Record());
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
    default:
      return false;
  }
}

ExpiringPointer<Function> ListParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore * ListParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}

}
