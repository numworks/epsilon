
#pragma once

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "store.h"

namespace Statistics::Categorical {

/* Controller for the parameter menu of the RF column headers.
 * Allows hiding of the column for the selected group */
class RFColumnParameterController
    : public Escher::ExplicitSelectableListViewController {
 public:
  RFColumnParameterController(Escher::Responder* parentResponder, Store* store,
                              Escher::StackViewController* stackViewController)
      : Escher::ExplicitSelectableListViewController(parentResponder),
        m_store(store),
        m_stackViewController(stackViewController) {
    m_hideRFColumnCell.label()->setMessage(I18n::Message::HideColumn);
  }

  // ExplicitSelectableListViewController
  bool handleEvent(Ion::Events::Event event) override;
  // NOTE: number of rows in the menu list, this has nothing to do with [setRow]
  int numberOfRows() const override { return k_numberOfCells; }
  const char* title() const override;
  const Escher::AbstractMenuCell* cell(int row) const override {
    assert(0 == row);
    return &m_hideRFColumnCell;
  }

  /* Set the group/col that is being selected on the tableData
   * Should be called before pushing this viewController on the stack */
  void setGroup(int group);

 private:
  constexpr static int k_numberOfCells = 1;
  constexpr static int k_titleBufferSize = 23 + 2;  // "RF" or "FR"
  // mutable because title() needs to be const
  mutable char m_titleBuffer[k_titleBufferSize];
  int m_group = -1;

  Escher::MenuCell<Escher::MessageTextView> m_hideRFColumnCell;
  Store* m_store;
  Escher::StackViewController* m_stackViewController;
};

}  // namespace Statistics::Categorical
