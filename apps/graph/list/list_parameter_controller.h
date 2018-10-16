#ifndef GRAPH_LIST_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_LIST_PARAM_CONTROLLER_H

#include <apps/shared/storage_list_parameter_controller.h>

namespace Graph {

class StorageListController;

class ListParameterController : public Shared::StorageListParameterController {
public:
  ListParameterController(StorageListController * listController, Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr) :
    Shared::StorageListParameterController(parentResponder, functionColorMessage, deleteFunctionMessage, tableDelegate),
    m_listController(listController),
    m_renameCell(I18n::Message::Rename)
  {}
  HighlightCell * reusableCell(int index) override;
protected:
  bool handleEnterOnRow(int rowIndex) override;
private:
  int totalNumberOfCells() const override {
    return Shared::StorageListParameterController::totalNumberOfCells() + 1;
  }
  void renameFunction();
  StorageListController * m_listController;
  MessageTableCell m_renameCell;
};

}

#endif
