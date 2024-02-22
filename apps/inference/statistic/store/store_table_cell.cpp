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
    m_header[i].setColor(Escher::Palette::Red);
    m_header[i].setEven(true);
    m_header[i].setFont(KDFont::Size::Small);
  }
}

void StoreTableCell::fillHeaderCellAtColumn(Escher::HighlightCell *cell,
                                            int column) {
  Shared::BufferFunctionTitleCell *headerCell =
      static_cast<Shared::BufferFunctionTitleCell *>(cell);
  assert(m_header <= headerCell &&
         headerCell < m_header + Table::k_maxNumberOfStoreColumns);
  fillColumnName(column, const_cast<char *>(headerCell->text()));
}

InputViewController *StoreTableCell::inputViewController() {
  return App::app()->inputViewController();
}

void StoreTableCell::reload() { recomputeDimensionsAndReload(true); }

CategoricalController *StoreTableCell::categoricalController() {
  return m_inputStoreController;
}

}  // namespace Inference
