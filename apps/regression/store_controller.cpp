#include "store_controller.h"
#include "app.h"
#include "regression_context.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Regression {

StoreController::StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Context * parentContext) :
  Shared::StoreController(parentResponder, inputEventHandlerDelegate, store, header),
  m_regressionContext(store, parentContext),
  m_storeParameterController(this, this)
  { }

bool StoreController::fillColumnWithFormula(Expression formula) {
  return privateFillColumnWithFormula(formula, RegressionContext::IsSymbol);
}

void StoreController::clearSelectedColumn() {
  m_store->deleteColumn(m_store->seriesAtColumn(selectedColumn()), m_store->relativeColumnIndex(selectedColumn()));
}

}
