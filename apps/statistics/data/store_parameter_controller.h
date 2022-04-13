#ifndef STATISTICS_STORE_PARAMETER_CONTROLLER_H
#define STATISTICS_STORE_PARAMETER_CONTROLLER_H

#include <apps/shared/store_parameter_controller.h>
#include <escher/message_table_cell_with_message_with_switch.h>

class StoreController;

namespace Statistics {

class StoreParameterController : public Shared::StoreParameterController {
public:
  StoreParameterController(Escher::Responder * parentResponder, Shared::StoreController * storeController);
  void initializeColumnParameters() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell * reusableCell(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  int numberOfCells() const override { return Shared::StoreParameterController::numberOfCells() + 1; }
private:
  constexpr static int k_indexOfCumulatedFrequencyCell = Shared::StoreParameterController::k_numberOfCells;

  I18n::Message sortMessage() override {
    return (m_columnIndex % 2 == 0) ? I18n::Message::SortValues : I18n::Message::SortSizes;
  }

  Escher::MessageTableCellWithMessageWithSwitch m_displayCumulatedFrequencyCell;
};

}

#endif
