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
  HistogramListController(Escher::Responder* parentResponder, Store* store,
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

  bool handleEvent(Ion::Events::Event event) override {
    // TODO: handle left/right events that update the series index.
    std::size_t seriesIndex = 0;

    if (!m_selectableListView.handleEvent(event)) {
      return false;
    }

    if (hasSelectedCell()) {
      /* If the SelectableListView handled the event by selecting a new cell,
       * then it took the firstResponder ownership. However we want
       * HistogramMainController to be the first responder, because the banner
       * view need to be updated as well. So the firstResponder ownership is
       * given back to HistogramMainController, which is the parent responder of
       * HistogramListController. */
      Escher::App::app()->setFirstResponder(parentResponder());

      /* Because SelectableListView lost the firstResponder ownership, the
       * SelectableTableView::willExitResponderChain function was called. This
       * function unhighlights the selected cell, so we need to set it
       * highlighted again. */
      setSelectedCellHighlight(true);

      // Set the current series and index in the snaphot
      setSelectedSeries(m_selectableListView.selectedRow());
      setSelectedSeriesIndex(seriesIndex);
    }

    return true;
  }

  // Helpers that can be used from the main controller
  void selectFirstCell() {
    /* Three actions are needed: selecting the first row in SelectableListView,
     * highlighting the selected cell, and updating the selected series in the
     * snapshot */
    m_selectableListView.selectFirstRow();
    setSelectedCellHighlight(true);
    // Set the current series and index in the snaphot
    setSelectedSeries(m_selectableListView.selectedRow());
    setSelectedSeriesIndex(0);
  }
  bool hasSelectedCell() {
    return m_selectableListView.selectedCell() != nullptr;
  }

  /* The selected cell in the SelectableListView can be highlighted or not. */
  void setSelectedCellHighlight(bool isHighlighted) {
    assert(hasSelectedCell());
    m_selectableListView.selectedCell()->setHighlighted(isHighlighted);
  }

  // Set and get the selected series or index from the Snapshot
  std::size_t selectedSeries() const;
  void setSelectedSeries(std::size_t selectedSeries);
  std::size_t selectedSeriesIndex() const;
  void setSelectedSeriesIndex(std::size_t selectedIndex);

 private:
  // Escher::TableViewDataSource
  KDCoordinate nonMemoizedRowHeight(int row) override { return 75; };

  // Maximum number of histograms displayed on the same screen
  constexpr static std::size_t k_displayedHistograms = 4;
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
