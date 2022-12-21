#ifndef STATISTICS_STORE_PARAMETER_CONTROLLER_H
#define STATISTICS_STORE_PARAMETER_CONTROLLER_H

#include <apps/shared/store_parameter_controller.h>
#include <escher/message_table_cell_with_message_with_switch.h>
#include <escher/message_table_cell_with_message.h>
#include "../store.h"

namespace Statistics {

class StoreController;

class StoreParameterController : public Shared::StoreParameterController {
public:
  StoreParameterController(Escher::Responder * parentResponder, StoreController * storeController, Store * m_store);
  void initializeColumnParameters() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  int typeAtIndex(int index) const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  /* When displayed, HideCumulatedFrequencyCell is last and second.
   * Remaining Shared::StoreParameterController are not displayed:
   * m_fillFormula, m_hideCell and m_clearColumn */
  constexpr static int k_indexOfHideCumulatedFrequencyCell = Shared::StoreParameterController::k_fillFormulaCellType;
  constexpr static int k_displayCumulatedFrequencyCellType = Shared::StoreParameterController::k_numberOfCells;
  constexpr static int k_hideCumulatedFrequencyCellType = k_displayCumulatedFrequencyCellType + 1;

  I18n::Message sortMessage() override {
    return (m_columnIndex % 2 == 0) ? I18n::Message::SortValues : I18n::Message::SortSizes;
  }

  Escher::MessageTableCellWithMessageWithSwitch m_displayCumulatedFrequencyCell;
  Escher::MessageTableCellWithMessage m_hideCumulatedFrequencyCell;
  Store * m_store;
  bool m_isCumulatedFrequencyColumnSelected;
};

}

#endif
