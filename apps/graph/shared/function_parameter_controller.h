#ifndef GRAPH_SHARED_FUNCTION_PARAMETER_CONTROLLER_H
#define GRAPH_SHARED_FUNCTION_PARAMETER_CONTROLLER_H

#include <apps/shared/list_parameter_controller.h>
#include <apps/exam_mode_configuration.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell_with_chevron_and_buffer.h>
#include "details_parameter_controller.h"
#include "domain_parameter_controller.h"
#include "../graph/graph_controller.h"

namespace Graph {

class FunctionParameterController : public Shared::ListParameterController {
public:
  FunctionParameterController(Escher::Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphController * graphController);
  void setRecord(Ion::Storage::Record record) override;
  // MemoizedListViewDataSource
  Escher::HighlightCell * cell(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override { return k_numberOfRows; }
private:
  // Shared cells + m_detailsCell + m_functionDomainCell + m_derivativeCell
  static constexpr int k_numberOfRows = Shared::ListParameterController::k_numberOfSharedCells + 3;
  bool displayDetails() const { return !ExamModeConfiguration::implicitPlotsAreForbidden() && m_detailsParameterController.detailsNumberOfSections() > 0; }
  bool displayDomain() const { return m_domainParameterController.isVisible() > 0; }
  Escher::MessageTableCellWithChevronAndMessage m_detailsCell;
  Escher::MessageTableCellWithChevronAndBuffer m_functionDomainCell;
  Escher::MessageTableCellWithMessageWithSwitch m_derivativeCell;
  DetailsParameterController m_detailsParameterController;
  DomainParameterController m_domainParameterController;
  GraphController * m_graphController;
};

}

#endif
