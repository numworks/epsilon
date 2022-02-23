#include "store_controller.h"
#include "statistics_context.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>
#include <float.h>
#include <cmath>
#include <poincare/print.h>

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

void StoreController::fillTitleCellText(HighlightCell * cell, int columnIndex) {
  assert(typeAtLocation(columnIndex, 0) == k_titleCellType);
  StoreTitleCell * myTitleCell = static_cast<StoreTitleCell *>(cell);
  char columnName[Shared::k_lengthOfColumnName];
  fillColumnName(columnIndex, columnName);
  char * title = const_cast<char *>(myTitleCell->text());
  I18n::Message titleType;
  if (columnIndex % 2 == 1) {
    titleType = I18n::Message::Frequencies;
  } else {
    titleType = I18n::Message::Values;
  }
  Poincare::Print::customPrintf(title, 50, I18n::translate(titleType), columnName);
  // TODO : the buffer size shouldn't be 50 but special characters take more buffer space than the displayed title.
  // The max characters displayed in the cell are 15 but the portuguese words take more buffer space.

}

}
