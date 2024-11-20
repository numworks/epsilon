#ifndef STATISTICS_HISTOGRAM_LIST_CONTROLLER_H
#define STATISTICS_HISTOGRAM_LIST_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/solid_color_cell.h>

#include "../store.h"
#include "histogram_range.h"

namespace Statistics {

class HistogramListController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::SelectableListViewDelegate {
 public:
  HistogramListController(Responder* parentResponder, Store* store,
                          uint32_t* storeVersion);

  // Escher::TableViewDataSource
  int numberOfRows() const override { return m_store->numberOfActiveSeries(); };

  // Escher::ListViewDataSource
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override { return 0; }
  Escher::SolidColorCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override {
    return std::size(m_displayCells);
    ;
  }

  /* TODO: override handleEvent so that it calls
   * selectableListView()->handleEvent, but returns the firstResponder
   * ownership to the HistogramMainController (i.e. parentResponder), and
   * restores the selected cell highlight. */

 private:
  // Escher::TableViewDataSource
  KDCoordinate nonMemoizedRowHeight(int row) override { return 75; };

  // Number of histograms displayed on the same screen
  constexpr static std::size_t k_displayedHistograms = 3;
  // SelectableList cells
  // TODO: replace with HistogramCells
  std::array<Escher::SolidColorCell, k_displayedHistograms> m_displayCells;

  // Model
  Store* m_store;
  uint32_t* m_storeVersion;
  HistogramRange m_histogramRange;
};

}  // namespace Statistics

#endif
