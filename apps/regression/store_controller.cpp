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
  return privateFillColumnWithFormula(formula, Symbol::isRegressionSymbol);
}

int StoreController::fillColumnName(int columnIndex, char * buffer) {
  int series = columnIndex / Store::k_numberOfColumnsPerSeries;
  int isXColumn = columnIndex % Store::k_numberOfColumnsPerSeries == 0;
  buffer[0] = isXColumn ? 'X' : 'Y';
  buffer[1] = static_cast<char>('1' + series);
  buffer[2] = 0;
  return 2;
}

void StoreController::clearSelectedColumn() {
  m_store->deleteColumn(m_store->seriesAtColumn(selectedColumn()), m_store->relativeColumnIndex(selectedColumn()));
}

}
