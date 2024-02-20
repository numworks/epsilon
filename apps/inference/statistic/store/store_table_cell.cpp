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

void StoreTableCell::fillColumnsNames() {
  for (int i = 0; i < k_maxNumberOfColumns; i++) {
    /* We delayed filling the column names X1, Y1 to ensure that the underlying
     * model was a valid DoublePairStore. */
    fillColumnName(i, const_cast<char *>(m_header[i].text()));
  }
}

int StoreTableCell::selectedSeries() {
  if (m_statistic->significanceTestType() == SignificanceTestType::Slope) {
    return static_cast<SlopeTStatistic *>(tableModel())->series();
  }
  assert(m_statistic->significanceTestType() == SignificanceTestType::OneMean);
  return static_cast<OneMeanStatistic *>(tableModel())->series();
}

void StoreTableCell::setSelectedSeries(int series) {
  if (m_statistic->significanceTestType() == SignificanceTestType::Slope) {
    return static_cast<SlopeTStatistic *>(tableModel())->setSeries(series);
  }
  assert(m_statistic->significanceTestType() == SignificanceTestType::OneMean);
  return static_cast<OneMeanStatistic *>(tableModel())->setSeries(series);
}

InputViewController *StoreTableCell::inputViewController() {
  return App::app()->inputViewController();
}

void StoreTableCell::reload() { recomputeDimensionsAndReload(true); }

CategoricalController *StoreTableCell::categoricalController() {
  return m_inputStoreController;
}

}  // namespace Inference
