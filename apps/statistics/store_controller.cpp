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

int StoreController::fillColumnName(int columnIndex, char * buffer) {
  int series = columnIndex / Store::k_numberOfColumnsPerSeries;
  int isValueColumn = columnIndex % Store::k_numberOfColumnsPerSeries == 0;
  buffer[0] = isValueColumn ? 'V' : 'N';
  buffer[1] = static_cast<char>('1' + series);
  buffer[2] = 0;
  return 2;
}

void StoreController::setTitleCellText(HighlightCell * cell, int columnIndex) {
  assert(typeAtLocation(columnIndex, 0) == k_titleCellType);
  StoreTitleCell * myTitleCell = static_cast<StoreTitleCell *>(cell);
  char columnName[Shared::ColumnParameterController::k_maxSizeOfColumnName];
  fillColumnName(columnIndex, columnName);
  char columnTitle[k_columnTitleSize]; // 50 is an ad-hoc value. A title cell can contain max 15 glyphs but the glyph can take more space than 1 byte in memory.
  I18n::Message titleType = columnIndex % 2 == 1 ? I18n::Message::Frequencies : I18n::Message::Values;
  Poincare::Print::customPrintf(columnTitle, k_columnTitleSize, I18n::translate(titleType), columnName);
  myTitleCell->setText(columnTitle);
}

void StoreController::clearSelectedColumn() {
  int series = seriesAtColumn(selectedColumn());
  int column = selectedColumn() % DoublePairStore::k_numberOfColumnsPerSeries;
  if (column == 0) {
    m_store->deleteAllPairsOfSeries(series);
  } else {
    m_store->resetColumn(series, column);
  }

}

}
