#include "store_controller.h"
#include "series_context.h"
#include "app.h"
#include "../apps_container.h"
#include "../constant.h"
#include <assert.h>
#include <float.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

StoreController::StoreController(Responder * parentResponder, Store * store, ButtonRowController * header) :
  Shared::StoreController(parentResponder, store, header),
  m_titleCells{},
  m_store(store)
{
}

void StoreController::setFormulaLabel() {
  int series = selectedColumn() / Store::k_numberOfColumnsPerSeries;
  int isValueColumn = selectedColumn() % Store::k_numberOfColumnsPerSeries == 0;
  char text[] = {isValueColumn ? 'V' : 'N', static_cast<char>('1' + series), '=', 0};
  static_cast<ContentView *>(view())->formulaInputView()->setBufferText(text);
}

void StoreController::fillColumnWithFormula(Expression * formula) {
  int currentColumn = selectedColumn();
  // Fetch the series used in the formula to compute the size of the filled in series
  char variables[7] = {0, 0, 0, 0, 0, 0, 0};
  formula->getVariables(Symbol::isSeriesSymbol, variables);
  int numberOfValuesToCompute = -1;
  int index = 0;
  while (variables[index] != 0) {
    const char * seriesName = Symbol::textForSpecialSymbols(variables[index]);
    assert(strlen(seriesName) == 2);
    int series = (int)(seriesName[1] - '0') - 1;
    assert(series >= 0 && series < FloatPairStore::k_numberOfSeries);
    if (numberOfValuesToCompute == -1) {
      numberOfValuesToCompute = m_store->numberOfPairsOfSeries(series);
    } else {
      numberOfValuesToCompute = min(numberOfValuesToCompute, m_store->numberOfPairsOfSeries(series));
    }
    index++;
  }
  if (numberOfValuesToCompute == -1) {
    numberOfValuesToCompute = m_store->numberOfPairsOfSeries(selectedColumn()/FloatPairStore::k_numberOfColumnsPerSeries);
  }

  SeriesContext seriesContext(m_store, const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext());
  for (int j = 0; j < numberOfValuesToCompute; j++) {
    // Set the context
    seriesContext.setSeriesPairIndex(j);
    // Compute the new value using the formula
    double evaluation = formula->approximateToScalar<double>(seriesContext);
    setDataAtLocation(evaluation, currentColumn, j + 1);
  }
  selectableTableView()->reloadData();
}

void StoreController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::StoreController::willDisplayCellAtLocation(cell, i, j);
  if (cellAtLocationIsEditable(i, j)) {
    return;
  }
  Shared::StoreTitleCell * mytitleCell = static_cast<Shared::StoreTitleCell *>(cell);
  bool isValuesColumn = i%Store::k_numberOfColumnsPerSeries == 0;
  mytitleCell->setSeparatorLeft(isValuesColumn);
  int seriesIndex = i/Store::k_numberOfColumnsPerSeries;
  assert(seriesIndex >= 0 && seriesIndex < FloatPairStore::k_numberOfSeries);
  if (isValuesColumn) {
    I18n::Message valuesMessages[] = {I18n::Message::Values1, I18n::Message::Values2, I18n::Message::Values3};
    mytitleCell->setText(I18n::translate(valuesMessages[seriesIndex]));
  } else {
    I18n::Message sizesMessages[] = {I18n::Message::Sizes1, I18n::Message::Sizes2, I18n::Message::Sizes3};
    mytitleCell->setText(I18n::translate(sizesMessages[seriesIndex]));
  }
  mytitleCell->setColor(m_store->numberOfPairsOfSeries(seriesIndex) == 0 ? Palette::GreyDark : Store::colorOfSeriesAtIndex(seriesIndex)); // TODO Share GreyDark with graph/list_controller
}

HighlightCell * StoreController::titleCells(int index) {
  assert(index >= 0 && index < k_numberOfTitleCells);
  return m_titleCells[index];
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
