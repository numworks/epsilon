#ifndef REGRESSION_STORE_PARAMETER_CONTROLLER_H
#define REGRESSION_STORE_PARAMETER_CONTROLLER_H

#include <apps/shared/store_parameter_controller.h>
#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "../store.h"

namespace Regression {

class StoreController;

class StoreParameterController : public Shared::StoreParameterController {
 public:
  StoreParameterController(Escher::Responder* parentResponder,
                           Shared::StoreColumnHelper* storeColumnHelper);
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override {
    return Shared::StoreParameterController::numberOfRows() + 1;
  }
  int typeAtIndex(int index) const override;
  Escher::AbstractMenuCell* reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  constexpr static int k_changeRegressionCellType =
      Shared::StoreParameterController::k_numberOfCells;
  constexpr static int k_changeRegressionCellIndex = 2;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_changeRegressionCell;
};

}  // namespace Regression

#endif
