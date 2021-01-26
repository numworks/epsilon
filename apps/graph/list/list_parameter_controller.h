#ifndef GRAPH_LIST_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_LIST_PARAM_CONTROLLER_H

#include <apps/shared/list_parameter_controller.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell_with_chevron_and_buffer.h>
#include "type_parameter_controller.h"
#include "domain_parameter_controller.h"

namespace Graph {

class ListController;

class ListParameterController : public Shared::ListParameterController {
public:
  ListParameterController(ListController * listController, Escher::Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate);
  bool handleEvent(Ion::Events::Event event) override;
  // SimpleListViewDataSource
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfRows() const override { return Shared::ListParameterController::numberOfRows() + k_localNumberOfCell; }
protected:
  bool handleEnterOnRow(int rowIndex) override;
private:
  void renameFunction();
  constexpr static int k_localNumberOfCell = 3;
  ListController * m_listController;
  Escher::MessageTableCellWithChevronAndMessage m_typeCell;
  Escher::MessageTableCellWithChevronAndBuffer m_functionDomain;
  Escher::MessageTableCell m_renameCell;
  TypeParameterController m_typeParameterController;
  DomainParameterController m_domainParameterController;
};

}

#endif
