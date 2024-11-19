#ifndef STATISTICS_HISTOGRAM_LIST_CONTROLLER_H
#define STATISTICS_HISTOGRAM_LIST_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/solid_color_cell.h>
#include <escher/tab_view_controller.h>

#include "../store.h"
#include "escher/selectable_list_view.h"
#include "histogram_range.h"
#include "statistics/graph/histogram_list_view.h"

namespace Statistics {

class HistogramListController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::SelectableListViewDelegate {
 public:
  HistogramListController(Responder* parentResponder, Store* store,
                          uint32_t* storeVersion);

  int numberOfRows() const override { return m_store->numberOfActiveSeries(); };
  int reusableCellCount(int type) const override {
    return std::size(m_displayCells);
    ;
  }
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override { return 0; }
  Escher::SolidColorCell* reusableCell(int index, int type) override;

 private:
  KDCoordinate nonMemoizedRowHeight(int row) override { return 75; };

  // number of histograms displayed on the same screen
  constexpr static std::size_t k_displayedHistograms = 3;

  std::array<Escher::SolidColorCell, k_displayedHistograms> m_displayCells;

  HistogramRange m_histogramRange;

  uint32_t* m_storeVersion;
  Store* m_store;
};

}  // namespace Statistics

#endif
