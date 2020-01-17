#include "store_controller.h"
#include "app.h"
#include "regression_context.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

StoreController::StoreController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, ButtonRowController * header, Context * parentContext) :
  Shared::StoreController(parentResponder, inputEventHandlerDelegate, store, header),
  m_titleCells{},
  m_regressionContext(store, parentContext),
  m_storeParameterController(this, store, this)
{
}

void StoreController::setFormulaLabel() {
  int series = selectedColumn() / Store::k_numberOfColumnsPerSeries;
  int isXColumn = selectedColumn() % Store::k_numberOfColumnsPerSeries == 0;
  char text[] = {isXColumn ? 'X' : 'Y', static_cast<char>('1' + series), '=', 0};
  static_cast<ContentView *>(view())->formulaInputView()->setBufferText(text);
}

bool StoreController::fillColumnWithFormula(Expression formula) {
  return privateFillColumnWithFormula(formula, Symbol::isRegressionSymbol);
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  ::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (typeAtLocation(i, j) != k_titleCellType) {
    return;
  }
  Shared::StoreTitleCell * mytitleCell = static_cast<Shared::StoreTitleCell *>(cell);
  bool isValuesColumn = i%Store::k_numberOfColumnsPerSeries == 0;
  mytitleCell->setSeparatorLeft(isValuesColumn && i > 0);
  int seriesIndex = i/Store::k_numberOfColumnsPerSeries;
  mytitleCell->setColor(m_store->numberOfPairsOfSeries(seriesIndex) == 0 ? Palette::GreyDark : Store::colorOfSeriesAtIndex(seriesIndex)); // TODO Share GreyDark with graph/list_controller and statistics/store_controller
  char name[] = {isValuesColumn ? 'X' : 'Y', static_cast<char>('1' + seriesIndex), 0};
  mytitleCell->setText(name);
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return &m_titleCells[index];
}

}
