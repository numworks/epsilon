#ifndef GRAPH_LIST_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_LIST_PARAM_CONTROLLER_H

#include <apps/shared/storage_list_parameter_controller.h>

namespace Graph {

class ListParameterController : public Shared::StorageListParameterController {
public:
  ListParameterController(Responder * parentResponder, Shared::StorageFunctionStore * functionStore, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr) :
    Shared::StorageListParameterController(parentResponder, functionStore, functionColorMessage, deleteFunctionMessage, tableDelegate),
    m_renameCell(I18n::Message::Rename)
  {}
  HighlightCell * reusableCell(int index) override;
protected:
  bool handleEnterOnRow(int rowIndex) override;
private:
  int totalNumberOfCells() const override {
    return Shared::StorageListParameterController::totalNumberOfCells() + 1;
  }
  MessageTableCell m_renameCell;
};

}

#endif
