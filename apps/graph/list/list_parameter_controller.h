#ifndef GRAPH_LIST_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_LIST_PARAM_CONTROLLER_H

#include <apps/shared/list_parameter_controller.h>

namespace Graph {

class ListController;

class ListParameterController : public Shared::ListParameterController {
public:
  ListParameterController(ListController * listController, Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr) :
    Shared::ListParameterController(parentResponder, functionColorMessage, deleteFunctionMessage, tableDelegate),
    m_listController(listController),
    m_renameCell(I18n::Message::Rename)
  {}
  // ListViewDataSource
  HighlightCell * reusableCell(int index, int type) override;
protected:
  bool handleEnterOnRow(int rowIndex) override;
private:
  int totalNumberOfCells() const override {
    return Shared::ListParameterController::totalNumberOfCells() + 1;
  }
  void renameFunction();
  ListController * m_listController;
  MessageTableCell m_renameCell;
};

}

#endif
