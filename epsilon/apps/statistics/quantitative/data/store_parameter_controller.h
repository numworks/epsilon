#pragma once

#include <apps/shared/store_parameter_controller.h>
#include <escher/menu_cell.h>
#include <escher/switch_view.h>

#include "../store.h"

namespace Statistics {

class StoreController;

class StoreParameterController
    : public Shared::ColorableStoreParameterController {
 public:
  StoreParameterController(Escher::Responder* parentResponder,
                           StoreController* storeController, Store* m_store);
  void initializeColumnParameters() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override {
    return m_isCumulatedFrequencyColumnSelected
               ? k_numberOfCellsIfCFColumnSelected
               : k_numberOfCellsIfCFColumnNotSelected;
  }
  const Escher::AbstractMenuCell* cell(int row) const override;
  void viewWillAppear() override;

 private:
  /* When Cumulated Frequency column is selected, only sortColumn and
   * hideCumulatedFrequencyCell cells are displayed. */
  constexpr static int k_numberOfCellsIfCFColumnSelected = 2;
  // Otherwise, add displayCumulatedFrequencyCell cell
  constexpr static int k_numberOfCellsIfCFColumnNotSelected =
      Shared::ColorableStoreParameterController::k_numberOfCells + 1;
  constexpr static int k_displayCFIndex =
      Shared::ColorableStoreParameterController::k_numberOfCells;

  I18n::Message sortMessage() override;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_displayCumulatedFrequencyCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView>
      m_hideCumulatedFrequencyCell;
  Store* m_store;
  bool m_isCumulatedFrequencyColumnSelected;
};

}  // namespace Statistics
