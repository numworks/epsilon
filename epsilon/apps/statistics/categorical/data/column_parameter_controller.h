#pragma once

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/switch_view.h>

#include "store.h"

namespace Statistics::Categorical {

class StoreController;

/* Controller for the parameter menu of the column headers.
 * Allows changing the name, toggle show/hide, clear and show RF column for the
 * selected group */
class ColumnParameterController
    : public Escher::ExplicitSelectableListViewController,
      public Escher::TextFieldDelegate {
 public:
  ColumnParameterController(Escher::Responder* parentResponder, Store* store,
                            Escher::StackViewController* stackViewController,
                            StoreController* storeController);

  // Escher::TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  // Escher::ExplicitSelectableListViewController
  int numberOfRows() const override { return k_numberOfCells; }
  bool handleEvent(Ion::Events::Event event) override;
  const Escher::AbstractMenuCell* cell(int row) const override;
  const char* title() const override;

  void setColumn(int col);

 private:
  constexpr static int k_numberOfCells = 4;
  constexpr static int k_titleBufferSize = 23 + sizeof(Store::Label);
  // mutable because title() needs to be const
  mutable char m_titleBuffer[k_titleBufferSize];
  int m_column = -1;

  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_columnNameCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_showInGraphCell;
  Escher::MenuCell<Escher::MessageTextView> m_clearColumnCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::SwitchView>
      m_relativeFreqCell;

  Store* m_store;
  Escher::StackViewController* m_stackViewController;
  StoreController* m_storeController;
};

}  // namespace Statistics::Categorical
