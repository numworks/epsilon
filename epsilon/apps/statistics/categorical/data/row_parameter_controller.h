#pragma once

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/menu_cell_with_editable_text.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "store.h"

namespace Statistics::Categorical {

class StoreController;

/* Controller for the parameter menu of the row headers.
 * Allows changing the name or clear for the selected category */
class RowParameterController
    : public Escher::ExplicitSelectableListViewController,
      public Escher::TextFieldDelegate {
 public:
  RowParameterController(Escher::Responder* parentResponder, Store* store,
                         Escher::StackViewController* stackViewController,
                         StoreController* storeController);

  // Responder via ExplicitSelectableListViewController
  bool handleEvent(Ion::Events::Event event) override;

  // ExplicitSelectableListViewController
  const Escher::AbstractMenuCell* cell(int row) const override;
  // NOTE: number of rows in the menu list, this has nothing to do with [setRow]
  int numberOfRows() const override { return k_numberOfCells; }
  const char* title() const override;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override {
    return TextFieldDelegate::textFieldShouldFinishEditing(textField, event) ||
           event == Ion::Events::Down;
  }
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;

  /* Set the row that is being selected on the tableData.
   * Should be called before pushing this viewController on the stack */
  void setRow(int row);

 private:
  constexpr static int k_numberOfCells = 2;
  constexpr static int k_titleBufferSize = 23 + sizeof(Store::Label);
  // mutable because title() needs to be const
  mutable char m_titleBuffer[k_titleBufferSize];
  int m_row = -1;

  Escher::MenuCellWithEditableText<Escher::MessageTextView> m_rowNameCell;
  Escher::MenuCell<Escher::MessageTextView> m_clearColumnCell;
  Store* m_store;
  Escher::StackViewController* m_stackViewController;
  StoreController* m_storeController;
};

}  // namespace Statistics::Categorical
