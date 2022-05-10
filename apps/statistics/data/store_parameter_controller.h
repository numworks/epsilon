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
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfCells() const override;
private:
  /* Number of Shared::StoreParameterController's cells to hide when cumulated
   * frequency column is selected : m_clearColumn, m_hideCell and m_fillFormula.
   */
  constexpr static int k_numberOfHiddenCells = 3;

  int indexOfCumulatedFrequencyCell() const { return numberOfCells() - 1; }
  bool isCumulatedFrequencyColumnSelected() const;
  I18n::Message sortMessage() override {
    return (m_columnIndex % 2 == 0) ? I18n::Message::SortValues : I18n::Message::SortSizes;
  }

  Escher::MessageTableCellWithMessageWithSwitch m_displayCumulatedFrequencyCell;
  Escher::MessageTableCellWithMessage m_hideCumulatedFrequencyCell;
  Store * m_store;
};

}

#endif
