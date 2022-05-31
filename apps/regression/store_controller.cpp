#include "store_controller.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

StoreController::StoreController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Context * parentContext) :
  Shared::StoreController(parentResponder, inputEventHandlerDelegate, store, header, parentContext),
  m_storeParameterController(this, this)
  { }

void StoreController::clearSelectedColumn() {
  m_store->deleteColumn(m_store->seriesAtColumn(selectedColumn()), m_store->relativeColumnIndex(selectedColumn()));
  selectCellAtLocation(selectedColumn(), 1);
}

InputViewController * StoreController::inputViewController() {
  return App::app()->inputViewController();
}

}
