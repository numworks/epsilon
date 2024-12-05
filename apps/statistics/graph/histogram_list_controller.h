#ifndef STATISTICS_HISTOGRAM_LIST_CONTROLLER_H
#define STATISTICS_HISTOGRAM_LIST_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/selectable_list_view_controller.h>

#include "../store.h"
#include "histogram_cell.h"
#include "histogram_range.h"

namespace Statistics {

class HistogramListController
    : public Escher::SelectableListViewController<Escher::ListViewDataSource>,
      public Escher::SelectableListViewDelegate {
 public:
  HistogramListController(Escher::Responder* parentResponder, Store* store,
                          HistogramRange* histogramRange);

  // Public API that can be used from the main controller

  /* If no statistics series was selected in the snapshot, this function selects
   * the first series and its first bar index. Otherwise, the currently selected
   * bar index is "sanitized" to ensure it is still in the authorized range and
   * that the selected bar is not empty. In all cases, when exiting this
   * function, you are guaranteed that selectedSeries() and selectedBarIndex()
   * return valid values. */
  void processSeriesAndBarSelection();

  void highlightRow(std::size_t row);

  void scrollAndHighlightHistogramBar(std::size_t row, std::size_t barIndex);

  // Unhighlight the entire list
  void unhighlightList() { m_selectableListView.deselectTable(); }

  // Get the selected series or index from the Snapshot
  std::size_t selectedSeries() const;
  std::size_t selectedBarIndex() const;

  // Height of one histogram graph (they all have the same size)
  KDCoordinate rowHeight() const {
    return numberOfRows() == 1 ? m_selectableListView.bounds().height() - 1
                               : k_listRowHeight;
  }

  // Escher::TableViewDataSource
  int numberOfRows() const override { return m_store->numberOfActiveSeries(); };

  // Escher::ListViewDataSource
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override { return 0; }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override {
    return m_displayCells.size();
  }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

 private:
  static constexpr KDCoordinate k_listRowHeight = 75;

  // Escher::TableViewDataSource
  // TODO: Escher::TableViewDataSource::nonMemoizedRowHeight should be const
  KDCoordinate nonMemoizedRowHeight(int row) override { return rowHeight(); }

  // Check if one of the statistics series is selected in the Snapshot
  bool hasSelectedSeries() const;

  // Set the selected series or index in the Snapshot
  void setSelectedSeries(std::size_t selectedSeries);
  void setSelectedBarIndex(std::size_t barIndex);

  /* Return the current bar index in the snapshot without checking the upper
   * bound */
  std::size_t unsafeSelectedBarIndex() const;

  // Navigation inside and between the histogram cells
  std::size_t horizontallyShiftedBarIndex(
      std::size_t previousBarIndex, std::size_t selectedSeries,
      OMG::HorizontalDirection direction) const;
  std::size_t sanitizeSelectedIndex(std::size_t selectedSeries,
                                    std::size_t initialSelectedIndex) const;
  std::size_t barIndexAfterSelectingNewSeries(
      std::size_t previousSelectedSeries, std::size_t currentSelectedSeries,
      std::size_t previousSelectedBarIndex) const;

  // Maximum number of histograms displayed on the same screen
  constexpr static std::size_t k_displayedHistograms = 4;
  // SelectableList cells
  std::array<HistogramCell, k_displayedHistograms> m_displayCells;

  // Model
  Store* m_store;
  HistogramRange* m_histogramRange;
};

}  // namespace Statistics

#endif
