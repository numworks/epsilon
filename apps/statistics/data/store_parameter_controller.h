#ifndef STATISTICS_STORE_PARAMETER_CONTROLLER_H
#define STATISTICS_STORE_PARAMETER_CONTROLLER_H

#include <apps/shared/store_parameter_controller.h>
#include <escher/menu_cell.h>
#include <escher/switch_view.h>

#include "../store.h"

namespace Statistics {

class StoreController;

class StoreParameterController : public Shared::StoreParameterController {
 public:
  StoreParameterController(Escher::Responder* parentResponder,
                           StoreController* storeController, Store* m_store);
  void initializeColumnParameters() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  Escher::AbstractMenuCell* cell(int index) override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

 private:
  /* When displayed, hideCumulatedFrequencyCell is last and second.
   * Remaining Shared::StoreParameterController are not displayed:
   * m_fillFormula, m_hideCell and m_clearColumn */
  constexpr static int k_hideCFIndex = 1;
  constexpr static int k_displayCFIndex =
      Shared::StoreParameterController::k_numberOfCells;

  I18n::Message sortMessage() override {
    return (m_column % 2 == 0) ? I18n::Message::SortValues
                               : I18n::Message::SortSizes;
  }

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_displayCumulatedFrequencyCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView>
      m_hideCumulatedFrequencyCell;
  Store* m_store;
  bool m_isCumulatedFrequencyColumnSelected;
};

}  // namespace Statistics

#endif
