#ifndef SEQUENCE_LIST_PARAM_CONTROLLER_H
#define SEQUENCE_LIST_PARAM_CONTROLLER_H

#include <escher/even_odd_expression_cell.h>
#include <escher/message_table_cell_with_chevron_and_expression.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell_with_editable_text.h>
#include <escher/message_table_cell_with_switch.h>
#include <escher/list_view_data_source.h>
#include "../../shared/list_parameter_controller.h"
#include "../../shared/parameter_text_field_delegate.h"
#include "../../shared/sequence.h"
#include "../../shared/sequence_store.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController;

class ListParameterController : public Shared::ListParameterController, public Escher::SelectableTableViewDelegate, public Shared::ParameterTextFieldDelegate {
public:
  ListParameterController(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, ListController * list);
  const char * title() override;

  bool textFieldShouldFinishEditing(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField * textField, const char * text, Ion::Events::Event event) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // MemoizedListViewDataSource
  Escher::HighlightCell * cell(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfRows() const override { return numberOfNonInheritedCells() + Shared::ListParameterController::numberOfRows(); }
private:
  void initialRankChanged(int value);
  bool handleEvent(Ion::Events::Event & event) override;
  int numberOfNonInheritedCells() const { return 2; } // number of non inherited cells
  Shared::Sequence * sequence() { return static_cast<Shared::Sequence *>(function().pointer()); }
  Escher::MessageTableCellWithChevronAndExpression m_typeCell;
  Escher::MessageTableCellWithEditableText m_initialRankCell;
  TypeParameterController m_typeParameterController;
};

}

#endif
