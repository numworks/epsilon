#pragma once

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
  const Escher::AbstractMenuCell* cell(int row) const override;
  void viewWillAppear() override;

 private:
  constexpr static int k_changeRegressionCellIndex = 2;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_changeRegressionCell;
};

}  // namespace Regression
