#ifndef STATISTICS_HISTOGRAM_LIST_CONTROLLER_H
#define STATISTICS_HISTOGRAM_LIST_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/solid_color_cell.h>
#include <escher/tab_view_controller.h>

#include "../store.h"
#include "histogram_main_view.h"
#include "histogram_range.h"
#include "statistics/graph/graph_button_row_delegate.h"

namespace Statistics {

class HistogramListController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::SelectableListViewDelegate,
      public GraphButtonRowDelegate {
 public:
  HistogramListController(Responder* parentResponder,
                          Escher::ButtonRowController* header,
                          Escher::TabViewController* tabController,
                          Escher::StackViewController* stackViewController,
                          Escher::ViewController* typeViewController,
                          Store* store, uint32_t* storeVersion);

  int numberOfRows() const override { return m_store->numberOfActiveSeries(); };

  Escher::SolidColorCell* reusableCell(int index, int type) override;

 private:
  KDCoordinate nonMemoizedRowHeight(int row) override { return 10; };
  ;

  HistogramMainView m_view;

  std::array<Escher::SolidColorCell, Store::k_numberOfSeries> m_cells;

  HistogramRange m_histogramRange;

  uint32_t* m_storeVersion;
  Store* m_store;
};

}  // namespace Statistics

#endif
