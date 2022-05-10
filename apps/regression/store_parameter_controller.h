#ifndef REGRESSION_STORE_PARAMETER_CONTROLLER_H
#define REGRESSION_STORE_PARAMETER_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include "regression_controller.h"
#include "store.h"
#include "../shared/store_parameter_controller.h"

namespace Regression {

class StoreController;

class StoreParameterController : public Shared::StoreParameterController {
public:
  StoreParameterController(Escher::Responder * parentResponder, StoreController * storeController);
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
protected:
  int numberOfCells() const override { return Shared::StoreParameterController::k_numberOfCells + 1; }

private:
  constexpr static int k_indexOfRegressionCell = Shared::StoreParameterController::k_numberOfCells;

  Escher::MessageTableCellWithChevronAndMessage m_changeRegressionCell;
};

}

#endif
