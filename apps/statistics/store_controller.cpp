#include "store_controller.h"
#include "statistics_context.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>
#include <float.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Context * parentContext) :
  Shared::StoreController(parentResponder, inputEventHandlerDelegate, store, header),
  m_store(store),
  m_statisticsContext(m_store, parentContext),
  m_storeParameterController(this, this)
  { }

bool StoreController::fillColumnWithFormula(Expression formula) {
  return privateFillColumnWithFormula(formula, Symbol::isSeriesSymbol);
}

bool StoreController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  if (std::fabs(floatBody) > FLT_MAX) {
    return false;
  }
  if (columnIndex % Store::k_numberOfColumnsPerSeries == 1) {
    if (floatBody < 0) {
      return false;
    }
  }
  return Shared::StoreController::setDataAtLocation(floatBody, columnIndex, rowIndex);
}

void StoreController::fillColumnName(int columnIndex, char * buffer) {
  int series = columnIndex / Store::k_numberOfColumnsPerSeries;
  int isValueColumn = columnIndex % Store::k_numberOfColumnsPerSeries == 0;
  buffer[0] = isValueColumn ? 'V' : 'N';
  buffer[1] = static_cast<char>('1' + series);
  for (int i = 2; i < Shared::k_lengthOfColumnName; i++) {
    buffer[i] = 0;
  }

}

}
