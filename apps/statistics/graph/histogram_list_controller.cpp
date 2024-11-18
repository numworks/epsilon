#include "histogram_list_controller.h"

#include "escher/highlight_cell.h"
#include "statistics/graph/graph_button_row_delegate.h"
#include "statistics/store.h"

namespace Statistics {

Escher::SolidColorCell makeColorCell(size_t index) {
  // return Escher::SolidColorCell(Store::colorOfSeriesAtIndex(index));
  return Escher::SolidColorCell();
}

HistogramListController::HistogramListController(
    Responder* parentResponder, Store* store, uint32_t* storeVersion,
    Escher::SelectableListView* listView)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder),
      m_displayCells({makeColorCell(0), makeColorCell(1), makeColorCell(2)}),
      m_histogramRange(store),
      m_storeVersion(storeVersion),
      m_store(store),
      m_listView(listView) {}

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
}

bool HistogramListController::handleEvent(Ion::Events::Event event) {
  return m_listView->handleEvent(event);
}

}  // namespace Statistics
