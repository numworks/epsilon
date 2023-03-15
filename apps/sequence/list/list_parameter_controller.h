#ifndef SEQUENCE_LIST_PARAM_CONTROLLER_H
#define SEQUENCE_LIST_PARAM_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/expression_view.h>
#include <escher/list_view_data_source.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "../../shared/list_parameter_controller.h"
#include "../../shared/parameter_text_field_delegate.h"
#include "../../shared/sequence.h"
#include "../../shared/sequence_store.h"
#include "type_parameter_controller.h"

namespace Sequence {

class ListController;

class ListParameterController : public Shared::ListParameterController,
                                public Escher::SelectableListViewDelegate,
                                public Shared::ParameterTextFieldDelegate {
 public:
  ListParameterController(
      Escher::InputEventHandlerDelegate *inputEventHandlerDelegate,
      ListController *list);
  const char *title() override;

  bool textFieldShouldFinishEditing(Escher::AbstractTextField *textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField *textField,
                                 const char *text,
                                 Ion::Events::Event event) override;
  void listViewDidChangeSelectionAndDidScroll(
      Escher::SelectableListView *l, int previousSelectedRow,
      bool withinTemporarySelection) override;

  // MemoizedListViewDataSource
  Escher::HighlightCell *cell(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell *cell, int index) override;
  int numberOfRows() const override {
    return numberOfNonInheritedCells() +
           Shared::ListParameterController::numberOfRows();
  }

 private:
  void initialRankChanged(int value);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfNonInheritedCells() const {
    return 2;
  }  // number of non inherited cells
  Shared::Sequence *sequence() {
    return static_cast<Shared::Sequence *>(function().pointer());
  }
  Escher::MenuCell<Escher::MessageTextView, Escher::ExpressionView,
                   Escher::ChevronView>
      m_typeCell;
  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_initialRankCell;
  TypeParameterController m_typeParameterController;
};

}  // namespace Sequence

#endif
