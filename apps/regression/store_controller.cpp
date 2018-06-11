#include "store_controller.h"
#include "app.h"
#include "regression_context.h"
#include "../apps_container.h"
#include "../constant.h"
#include "../../poincare/src/layout/char_layout.h"
#include "../../poincare/src/layout/horizontal_layout.h"
#include "../../poincare/src/layout/vertical_offset_layout.h"
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

StoreController::StoreController(Responder * parentResponder, Store * store, ButtonRowController * header) :
  Shared::StoreController(parentResponder, store, header),
  m_titleCells{},
  m_regressionContext(store)
{
}

StoreContext * StoreController::storeContext() {
  m_regressionContext.setParentContext(const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext());
  return &m_regressionContext;
}

void StoreController::setFormulaLabel() {
  int series = selectedColumn() / Store::k_numberOfColumnsPerSeries;
  int isXColumn = selectedColumn() % Store::k_numberOfColumnsPerSeries == 0;
  char text[] = {isXColumn ? 'X' : 'Y', static_cast<char>('1' + series), '=', 0};
  static_cast<ContentView *>(view())->formulaInputView()->setBufferText(text);
}

void StoreController::fillColumnWithFormula(Expression * formula) {
  privateFillColumnWithFormula(formula, Symbol::isRegressionSymbol);
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  ::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  Shared::StoreTitleCell * mytitleCell = static_cast<Shared::StoreTitleCell *>(cell);
  bool isValuesColumn = i%Store::k_numberOfColumnsPerSeries == 0;
  mytitleCell->setSeparatorLeft(isValuesColumn);
  int seriesIndex = i/Store::k_numberOfColumnsPerSeries;
  mytitleCell->setColor(m_store->numberOfPairsOfSeries(seriesIndex) == 0 ? Palette::GreyDark : Store::colorOfSeriesAtIndex(seriesIndex)); // TODO Share GreyDark with graph/list_controller and statistics/store_controller
  char name[] = {isValuesColumn ? 'X' : 'Y', static_cast<char>('1' + seriesIndex), 0};
  mytitleCell->setText(name);
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return m_titleCells[index];
}

View * StoreController::loadView() {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    m_titleCells[i] = new Shared::StoreTitleCell();
  }
  return Shared::StoreController::loadView();
}

void StoreController::unloadView(View * view) {
  for (int i = 0; i < k_numberOfTitleCells; i++) {
    delete m_titleCells[i];
    m_titleCells[i] = nullptr;
  }
  Shared::StoreController::unloadView(view);
}

}
