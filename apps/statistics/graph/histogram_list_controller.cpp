#include "histogram_list_controller.h"

#include "escher/solid_color_view.h"
#include "statistics/graph/graph_button_row_delegate.h"
#include "statistics/store.h"

namespace Statistics {

Escher::SolidColorCell makeColorCell(size_t index) {
  Escher::SolidColorCell cell;
  cell.setColor(Store::colorLightOfSeriesAtIndex(index));
  cell.setHighlightColor(Store::colorOfSeriesAtIndex(index));
  return cell;
}

HistogramListController::HistogramListController(Responder* parentResponder,
                                                 Store* store,
                                                 uint32_t* storeVersion)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder, this),
      m_displayCells({makeColorCell(0), makeColorCell(1), makeColorCell(2)}),
      m_histogramRange(store),
      m_storeVersion(storeVersion),
      m_store(store) {
  auto listBounds = m_selectableListView.bounds();
}

Escher::SolidColorCell* HistogramListController::reusableCell(int index,
                                                              int type) {
  assert(type == 0);
  assert(index >= 0 && index < std::size(m_displayCells));
  return &m_displayCells[index];
}

void HistogramListController::fillCellForRow(Escher::HighlightCell* cell,
                                             int row) {
  assert(row >= 0 && row < numberOfRows());
  Escher::SolidColorCell* colorCell =
      static_cast<Escher::SolidColorCell*>(cell);
  colorCell->setColor(Store::colorOfSeriesAtIndex(row));
  colorCell->setHighlightColor(Store::colorLightOfSeriesAtIndex(row));
}

}  // namespace Statistics
