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
  m_titleCells{}
{
}

void StoreController::setFormulaLabel() {
  int series = selectedColumn() / Store::k_numberOfColumnsPerSeries;
  int isXColumn = selectedColumn() % Store::k_numberOfColumnsPerSeries == 0;
  char text[] = {isXColumn ? 'X' : 'Y', static_cast<char>('1' + series), '=', 0};
  static_cast<ContentView *>(view())->formulaInputView()->setBufferText(text);
}

void StoreController::fillColumnWithFormula(Expression * formula) {
  int currentColumn = selectedColumn();
  // Fetch the series used in the formula to compute the size of the filled in series
  char variables[7] = {0, 0, 0, 0, 0, 0, 0};
  formula->getVariables(Symbol::isRegressionSymbol, variables);
  int numberOfValuesToCompute = -1;
  int index = 0;
  while (variables[index] != 0) {
    const char * seriesName = Symbol::textForSpecialSymbols(variables[index]);
    assert(strlen(seriesName) == 2);
    int series = (int)(seriesName[1] - '0') - 1;
    assert(series >= 0 && series < DoublePairStore::k_numberOfSeries);
    if (numberOfValuesToCompute == -1) {
      numberOfValuesToCompute = m_store->numberOfPairsOfSeries(series);
    } else {
      numberOfValuesToCompute = min(numberOfValuesToCompute, m_store->numberOfPairsOfSeries(series));
    }
    index++;
  }
  if (numberOfValuesToCompute == -1) {
    numberOfValuesToCompute = m_store->numberOfPairsOfSeries(selectedColumn()/DoublePairStore::k_numberOfColumnsPerSeries);
  }

  RegressionContext regressionContext(m_store, const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext());
  for (int j = 0; j < numberOfValuesToCompute; j++) {
    // Set the context
    regressionContext.setSeriesPairIndex(j);
    // Compute the new value using the formula
    double evaluation = formula->approximateToScalar<double>(regressionContext);
    setDataAtLocation(evaluation, currentColumn, j + 1);
  }
  selectableTableView()->reloadData();
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
