#ifndef SEQUENCE_LIST_PARAM_CONTROLLER_H
#define SEQUENCE_LIST_PARAM_CONTROLLER_H

#include "../../shared/list_parameter_controller.h"
#include "../../shared/parameter_text_field_delegate.h"
#include "../sequence.h"
#include "../sequence_store.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController;

class ListParameterController : public Shared::ListParameterController, public SelectableTableViewDelegate, public Shared::ParameterTextFieldDelegate {
public:
  ListParameterController(::InputEventHandlerDelegate * inputEventHandlerDelegate, ListController * list);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // ListViewDataSource
  HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
#if FUNCTION_COLOR_CHOICE
  constexpr static int k_totalNumberOfCell = 5;
#else
  constexpr static int k_totalNumberOfCell = 4;
#endif
  int totalNumberOfCells() const override;
  Sequence * sequence() { return static_cast<Sequence *>(function().pointer()); }
  bool hasInitialRankRow() const;
  MessageTableCellWithChevronAndExpression m_typeCell;
  MessageTableCellWithEditableText m_initialRankCell;
  TypeParameterController m_typeParameterController;
};

}

#endif
