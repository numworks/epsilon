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
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());

  if (cell == &m_enableCell && m_enableCell.ShouldEnterOnEvent(event)) {
    function()->setActive(!function()->isActive());
    resetMemoization();
    m_selectableTableView.reloadData();
    return true;
  }
  if (cell == &m_colorCell && m_colorCell.ShouldEnterOnEvent(event)) {
    m_colorParameterController.setRecord(m_record);
    stack->push(&m_colorParameterController);
    return true;
  }
  if (cell == &m_deleteCell && m_deleteCell.ShouldEnterOnEvent(event)) {
    assert(functionStore()->numberOfModels() > 0);
    m_selectableTableView.deselectTable();
    functionStore()->removeModel(m_record);
    setRecord(Ion::Storage::Record());
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->popUntilDepth(Shared::InteractiveCurveViewController::k_graphControllerStackDepth, true);
    return true;
  }
  return false;
}

ExpiringPointer<Function> ListParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore * ListParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}

}
