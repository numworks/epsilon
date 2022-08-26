#include "list_parameter_controller.h"
#include "function_app.h"
#include "color_names.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ExplicitSelectableListViewController(parentResponder, tableDelegate),
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
    m_enableCell.setState(function()->isActive());
  }
  if (cell == &m_colorCell) {
    m_colorCell.setMessage(I18n::Message::Color);
    m_colorCell.setSubtitle(ColorNames::NameForColor(function()->color()));
  }
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectCellAtLocation(0, 0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell * cell = selectedCell();
  if (cell == &m_enableCell) {
    return m_enableCell.handleEvent(event, this, &ListParameterController::enableSwitched);
  }
  if (cell == &m_colorCell) {
    return m_colorCell.handleEvent(event, this, &ListParameterController::colorPressed);
  }
  if (cell == &m_deleteCell) {
    return m_deleteCell.handleEvent(event, this, &ListParameterController::deletePressed);
  }
  return false;
}

void ListParameterController::enableSwitched(bool enable) {
  function()->setActive(enable);
  resetMemoization();
  m_selectableTableView.reloadData();
}

void ListParameterController::colorPressed() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  m_colorParameterController.setRecord(m_record);
  stack->push(&m_colorParameterController);
}

void ListParameterController::deletePressed() {
  assert(functionStore()->numberOfModels() > 0);
  m_selectableTableView.deselectTable();
  functionStore()->removeModel(m_record);
  setRecord(Ion::Storage::Record());
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
}

ExpiringPointer<Function> ListParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore * ListParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}

}
