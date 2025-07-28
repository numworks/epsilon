#include "list_parameter_controller.h"

#include <assert.h>

#include "color_names.h"
#include "function_app.h"

using namespace Escher;

namespace Shared {

ListParameterController::ListParameterController(
    Responder* parentResponder, I18n::Message functionColorMessage,
    I18n::Message deleteFunctionMessage,
    SelectableListViewDelegate* listDelegate)
    : ExplicitSelectableListViewController(parentResponder, listDelegate),
      m_colorParameterController(nullptr) {
  m_enableCell.label()->setMessage(
      I18n::Message::ActivateDeactivateListParamTitle);
  m_enableCell.subLabel()->setMessage(
      I18n::Message::ActivateDeactivateListParamDescription);
  m_deleteCell.label()->setMessage(deleteFunctionMessage);
  m_colorCell.label()->setMessage(I18n::Message::Color);
}

void ListParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_colorCell.subLabel()->setMessage(
      ColorNames::NameForColor(function()->color()));
  updateEnableCellSwitch();
  if (selectedRow() == -1) {
    selectRow(0);
  } else {
    selectRow(selectedRow());
  }
  m_selectableListView.reloadData();
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectRow(0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  HighlightCell* cell = selectedCell();
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());

  if (cell == &m_enableCell && m_enableCell.canBeActivatedByEvent(event)) {
    function()->setActive(!function()->isActive());
    updateEnableCellSwitch();
    m_selectableListView.reloadSelectedCell();
    return true;
  }
  if (cell == &m_colorCell && m_colorCell.canBeActivatedByEvent(event)) {
    m_colorParameterController.setRecord(m_record);
    stack->push(&m_colorParameterController);
    return true;
  }
  if (cell == &m_deleteCell && m_deleteCell.canBeActivatedByEvent(event)) {
    assert(functionStore()->numberOfModels() > 0);
    m_selectableListView.deselectTable();
    functionStore()->removeModel(m_record);
    StackViewController* stack =
        static_cast<StackViewController*>(parentResponder());
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        true);
    setRecord(Ion::Storage::Record());
    return true;
  }
  return false;
}

ExpiringPointer<Function> ListParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore* ListParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}

void ListParameterController::updateEnableCellSwitch() {
  if (!m_record.isNull()) {
    m_enableCell.accessory()->setState(function()->isActive());
  }
}

}  // namespace Shared
