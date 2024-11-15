#include "histogram_list_controller.h"

#include "statistics/graph/graph_button_row_delegate.h"
#include "statistics/store.h"

namespace Statistics {

Escher::SolidColorCell makeColorCell(size_t index) {
  return Escher::SolidColorCell(Store::colorOfSeriesAtIndex(index));
}

HistogramListController::HistogramListController(
    Responder* parentResponder, Escher::ButtonRowController* header,
    Escher::TabViewController* tabController,
    Escher::StackViewController* stackViewController,
    Escher::ViewController* typeViewController, Store* store,
    uint32_t* storeVersion)
    : Escher::SelectableListViewController<Escher::ListViewDataSource>(
          parentResponder),
      GraphButtonRowDelegate(header, stackViewController, this,
                             typeViewController),
      m_view(store, &m_histogramRange, this, this, this, this),
      m_cells({makeColorCell(0), makeColorCell(1), makeColorCell(2),
               makeColorCell(3), makeColorCell(4), makeColorCell(5)}),
      m_histogramRange(store),
      m_storeVersion(storeVersion),
      m_store(store) {}

Escher::SolidColorCell* HistogramListController::reusableCell(int index,
                                                              int type) {
  assert(type == 0);
  assert(index >= 0);
  assert(index < std::size(m_cells));
  return &m_cells[index];
}

}  // namespace Statistics
