#ifndef SEQUENCE_LIST_CONTROLLER_H
#define SEQUENCE_LIST_CONTROLLER_H

#include <escher.h>
#include "../sequence_title_cell.h"
#include "../sequence_store.h"
#include "sequence_expression_cell.h"
#include "../../shared/new_function_cell.h"
#include "../../shared/list_controller.h"
#include "../../shared/list_parameter_controller.h"

namespace Sequence {

class ListController : public Shared::ListController, public SelectableTableViewDelegate {
public:
  ListController(Responder * parentResponder, SequenceStore * sequenceStore, HeaderViewController * header);
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
private:
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  TableViewCell * titleCells(int index) override;
  TableViewCell * expressionCells(int index) override;
  constexpr static int k_maxNumberOfRows = 3;
  SequenceTitleCell m_functionTitleCells[k_maxNumberOfRows];
  SequenceExpressionCell m_expressionCells[k_maxNumberOfRows];
  Shared::ListParameterController m_parameterController;
};

}

#endif
