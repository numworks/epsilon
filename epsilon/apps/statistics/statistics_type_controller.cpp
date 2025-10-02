
#include "statistics_type_controller.h"

#include "statistics_type_view_model.h"

namespace Statistics {

DataTypeController::DataTypeController(
    Escher::Responder* parentResponder, Escher::StackViewController* stackView,
    DataTypeViewModel* dataTypeModel,
    Escher::ModalViewController* rootViewController,
    Escher::TabViewDataSource* tabViewDataSource)
    : Escher::UniformSelectableListController<DataTypeCell, k_numberOfCells>(
          parentResponder),
      m_stackViewController(stackView),
      m_rootViewController(rootViewController),
      m_tabViewDataSource(tabViewDataSource),
      m_dataTypeModel(dataTypeModel) {
  selectRow(DataTypeViewModel::IndexOfDataTypeView(
      m_dataTypeModel->selectedDataType()));
  for (uint8_t i = 0; i < DataTypeViewModel::k_numberOfDataTypes; i++) {
    DataTypeViewModel::DataType dataType =
        DataTypeViewModel::DataTypeAtIndex(i);
    cell(i)->subLabel()->setMessage(
        DataTypeViewModel::SubtitleForDataType(dataType));
    cell(i)->label()->setMessage(
        DataTypeViewModel::MessageForDataType(dataType));
  }
}

void DataTypeController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    selectRow(DataTypeViewModel::IndexOfDataTypeView(
        m_dataTypeModel->selectedDataType()));
    m_selectableListView.reloadData();
  } else {
    UniformSelectableListController::handleResponderChainEvent(event);
  }
}

bool DataTypeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE ||
      event == Ion::Events::Right) {
    DataTypeViewModel::DataType old = m_dataTypeModel->selectedDataType();
    m_dataTypeModel->selectDataType(
        DataTypeViewModel::DataTypeAtIndex(selectedRow()));
    m_stackViewController->pop();
    if (old == m_dataTypeModel->selectedDataType()) {
      return true;
    }

    m_rootViewController->refetchMainView();
    m_rootViewController->viewWillAppear();
    Escher::App::app()->setFirstResponder(m_rootViewController);
    return true;
  }
  return false;
}
}  // namespace Statistics
