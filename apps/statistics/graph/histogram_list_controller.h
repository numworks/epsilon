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

  // API that can be used from the main controller

  /* If no statistics series was selected in the snapshot, this function selects
   * the first series and its first bar index. Otherwise, the currently selected
   * bar index is "sanitized" to ensure it is still in the authorized range and
   * that the selected bar is not empty. In all cases, when exiting this
   * function, selectedSeries() and selectedBarIndex() are guaranteed to return
   * valid values. */
  void processSeriesAndBarSelection();

  void highlightSelectedSeries();

  void scrollAndHighlightHistogramBar(int row, int16_t barIndex);

  void unhighlightList() { m_selectableListView.deselectTable(); }

  // Get the selected series or index from the Snapshot
  int8_t selectedSeries() const;
  int16_t selectedBarIndex() const;

  // Height of one histogram graph (they all have the same size)
  KDCoordinate rowHeight() const {
    return numberOfRows() == 1 ? m_selectableListView.bounds().height() - 1
                               : k_rowHeight;
  }

  // Escher::TableViewDataSource
  int numberOfRows() const override { return m_store->numberOfActiveSeries(); }

  // Escher::ListViewDataSource
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  int typeAtRow(int row) const override { return 0; }
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override {
    return static_cast<int>(m_displayCells.size());
  }

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

 private:
  static constexpr KDCoordinate k_rowHeight = 75;

  // Escher::TableViewDataSource
  KDCoordinate nonMemoizedRowHeight(int row) override { return rowHeight(); }

  void setMainControllerFirstResponder() const;

  // Check if one of the statistics series is selected in the Snapshot
  bool hasSelectedSeries() const;

  // Set the selected series or index in the Snapshot
  void setSelectedSeries(int8_t series);
  void setSelectedBarIndex(int16_t barIndex);

  /* Return the current bar index in the snapshot without checking the upper
   * bound */
  int16_t unsafeSelectedBarIndex() const;

  // Navigation inside and between the histogram cells
  bool moveSelectionHorizontally(OMG::HorizontalDirection direction);

  int16_t sanitizedSelectedIndex(int8_t selectedSeries,
                                 int16_t initialSelectedIndex) const;
  int16_t barIndexAfterSelectingNewSeries(
      int8_t previousSelectedSeries, int8_t currentSelectedSeries,
      int16_t previousSelectedBarIndex) const;

  // Maximum number of histograms displayed on the same screen
  constexpr static size_t k_displayedHistograms = 4;
  // SelectableList cells
  std::array<HistogramCell, k_displayedHistograms> m_displayCells;

  // Model
  Store* m_store;
  HistogramRange* m_histogramRange;
};

}  // namespace Statistics

#endif
