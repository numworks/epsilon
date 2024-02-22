#include "store_table_cell.h"

#include <escher/palette.h>

#include "inference/app.h"
#include "inference/statistic/chi_square_and_slope/input_goodness_controller.h"
#include "input_store_controller.h"

using namespace Escher;

namespace Inference {

StoreTableCell::StoreTableCell(Responder *parentResponder, Statistic *statistic,
                               Poincare::Context *parentContext,
                               InputStoreController *inputStoreController)
    : DoubleColumnTableCell(parentResponder, statistic),
      StoreColumnHelper(this, parentContext, this),
      m_inputStoreController(inputStoreController) {
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    m_header[i].setEven(true);
    m_header[i].setFont(KDFont::Size::Small);
  }
}

bool StoreTableCell::textFieldDidFinishEditing(
    Escher::AbstractTextField *textField, Ion::Events::Event event) {
  Table *t = tableModel();
  if (t->numberOfSeries() == 1) {
    return DoubleColumnTableCell::textFieldDidFinishEditing(textField, event);
  }

  assert(t->numberOfSeries() == 2);
  Shared::DoublePairStore *s = store();
  int minLength = std::min<int>(s->numberOfPairsOfSeries(t->seriesAt(0)),
                                s->numberOfPairsOfSeries(t->seriesAt(1)));

  if (!DoubleColumnTableCell::textFieldDidFinishEditing(textField, event)) {
    return false;
  }
  /* Even though the table size may not have changed, reload to update the
   * visibility of the cells if the shorter series length has changed. */

  int minLength2 = std::min<int>(s->numberOfPairsOfSeries(t->seriesAt(0)),
                                 s->numberOfPairsOfSeries(t->seriesAt(1)));
  if (minLength != minLength2) {
    reload();
  }
  return true;
}

void StoreTableCell::fillCellForLocation(Escher::HighlightCell *cell,
                                         int column, int row) {
  DoubleColumnTableCell::fillCellForLocation(cell, column, row);

  if (typeAtLocation(column, row) == k_typeOfHeaderCells) {
    Shared::BufferFunctionTitleCell *headerCell =
        static_cast<Shared::BufferFunctionTitleCell *>(cell);
    assert(m_header <= headerCell &&
           headerCell < m_header + Table::k_maxNumberOfStoreColumns);
    headerCell->setColor(
        store()->colorOfSeriesAtIndex(store()->seriesAtColumn(column)));
    fillColumnName(column, const_cast<char *>(headerCell->text()));
    return;
  }

  assert(typeAtLocation(column, row) == k_typeOfInnerCells);
  /* Hide cells to avoid padding a shorter series with empty cells.
   * FIXME This is a hack to simulate the behavior of a StoreController, the
   * classes should be factorized. */
  cell->setVisible(row - 1 <= store()->numberOfPairsOfSeries(
                                  store()->seriesAtColumn(column)));
}

InputViewController *StoreTableCell::inputViewController() {
  return App::app()->inputViewController();
}

void StoreTableCell::reload() { recomputeDimensionsAndReload(true); }

CategoricalController *StoreTableCell::categoricalController() {
  return m_inputStoreController;
}

}  // namespace Inference
