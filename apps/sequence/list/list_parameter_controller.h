#ifndef SEQUENCE_LIST_PARAM_CONTROLLER_H
#define SEQUENCE_LIST_PARAM_CONTROLLER_H

#include <apps/shared/list_parameter_controller.h>
#include <apps/shared/parameter_text_field_delegate.h>
#include <apps/shared/sequence.h>
#include <apps/shared/sequence_store.h>
#include <escher/chevron_view.h>
#include <escher/even_odd_expression_cell.h>
#include <escher/layout_view.h>
#include <escher/list_view_data_source.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/message_text_view.h>

#include "type_parameter_controller.h"

namespace Sequence {

class ListController;

class ListParameterController : public Shared::ListParameterController,
                                public Escher::SelectableListViewDelegate,
                                public Shared::ParameterTextFieldDelegate {
 public:
  ListParameterController(ListController* list);
  const char* title() const override;

  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  void listViewDidChangeSelectionAndDidScroll(
      Escher::SelectableListView* l, int previousSelectedRow,
      KDPoint previousOffset, bool withinTemporarySelection) override;

  // MemoizedListViewDataSource
  Escher::HighlightCell* cell(int row) override;
  void viewWillAppear() override;
  int numberOfRows() const override {
    return numberOfNonInheritedCells() +
           Shared::ListParameterController::numberOfRows();
  }

 private:
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfNonInheritedCells() const {
    return 2;
  }  // number of non inherited cells

  void updateFirstRankCell();

  Shared::Sequence* sequence() {
    return static_cast<Shared::Sequence*>(function().pointer());
  }
  Escher::MenuCell<Escher::MessageTextView, Escher::LayoutView,
                   Escher::ChevronView>
      m_typeCell;
  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_firstRankCell;
  TypeParameterController m_typeParameterController;
};

}  // namespace Sequence

#endif
