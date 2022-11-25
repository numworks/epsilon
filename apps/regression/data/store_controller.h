#ifndef REGRESSION_STORE_CONTROLLER_H
#define REGRESSION_STORE_CONTROLLER_H

#include "../store.h"
#include "store_parameter_controller.h"
#include <apps/shared/store_controller.h>

namespace Regression {

class StoreController : public Shared::StoreController {
public:
  StoreController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Escher::ButtonRowController * header, Poincare::Context * parentContext);
  Model * selectedModel() { return static_cast<Store *>(m_store)->modelForSeries(selectedSeries()); }

private:
  Escher::InputViewController * inputViewController() override;
  Escher::SelectableViewController * columnParameterController() override { return &m_storeParameterController; }
  Shared::ColumnParameters * columnParameters() override { return &m_storeParameterController; }
  void clearSelectedColumn() override;
  StoreParameterController m_storeParameterController;
};

}

#endif
