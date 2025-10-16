
#include "statistics_type_controller.h"

#include "app.h"
#include "statistics_type_view_model.h"

namespace Statistics {

DataTypeController::DataTypeController(Escher::Responder* parentResponder,
                                       Escher::StackViewController* stackView,
                                       DataTypeViewModel* dataTypeModel)
    : Escher::UniformSelectableListController<DataTypeCell, k_numberOfCells>(
          parentResponder),
      m_stackViewController(stackView),
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
    DataTypeViewModel::DataType newDataType =
        DataTypeViewModel::DataTypeAtIndex(selectedRow());
    if (m_dataTypeModel->selectedDataType() != newDataType) {
      /* NOTE: switching active variant before changing type in model.
       * See comment in [switchActiveVariant] */
      Statistics::App::app()->m_tabs.tab<App::StoreTab>()->switchActiveVariant(
          newDataType);
      m_dataTypeModel->selectDataType(newDataType);
    }
    m_stackViewController->pop();
    return true;
  }
  return false;
}
}  // namespace Statistics
